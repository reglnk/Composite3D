#ifndef CM3D_GRAPHICS_HPP
#define CM3D_GRAPHICS_HPP

#include <cm3d/Tl/Aliases.hpp>
#include <cm3d/Tl/String.hpp>
#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Core/FileSystem.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cstdint>
#include <stdexcept>

namespace cm3d
{
	namespace Graphics
	{
		class Texture2D
		{
		protected:
			stbi_uc *mData = nullptr;
			int mWidth = 0;
			int mHeight = 0;
			int mChannels = 0;
			
		public:
			aiTextureType mType = aiTextureType_NONE;
			FileSystem::sPath path;
			
			inline Texture2D() = default;
			inline Texture2D(aiTextureType type): mType(type) {}
			inline Texture2D(FileSystem::sPath const &p): path(p) {}
			inline Texture2D(aiTextureType type, FileSystem::sPath const &p): mType(type), path(p) {}
			
			inline Texture2D(Texture2D const &oth):
				mWidth(oth.mWidth), mHeight(oth.mHeight), mChannels(oth.mChannels),
				mType(oth.mType), path(oth.path)
			{
				const size_t memsz = mWidth * mHeight * mChannels;
				mData = (stbi_uc *)mAlloc(memsz);
				memcpy(mData, oth.mData, memsz);
			}
			inline Texture2D(Texture2D &&rv):
				mData(rv.mData), mWidth(rv.mWidth), mHeight(rv.mHeight), mChannels(rv.mChannels),
				mType(rv.mType), path(std::move(rv.path))
			{ rv.mData = nullptr; }
			
			inline ~Texture2D() noexcept {
				if (mData)
					stbi_image_free(mData);
			}
			
			constexpr inline auto type() const { return mType; }
			constexpr inline auto data() const { return mData; }
			constexpr inline auto width() const { return mWidth; }
			constexpr inline auto height() const { return mHeight; }
			constexpr inline auto channels() const { return mChannels; }
			
			inline operator bool() const {
				return mData;
			}
			
			inline stbi_uc *load() {
				FileSystem::sPath nat = path;
				FileSystem::toNative(&nat);
				return mData = stbi_load(nat.c_str(), &mWidth, &mHeight, &mChannels, 0);
			}
		};
		
		struct Material
		{
			glm::vec3 diffuse;
			glm::vec3 specular;
			glm::vec3 ambient;
			glm::vec3 transparent;
			glm::vec3 emissive;
			
			float metalness;
			float roughness;
			float opacity;
			
			String name;
			DynArray<uint32_t> texIndex;
		};
		
		class Mesh
		{
		protected:
			DynArray<glm::vec3> vertices;
			DynArray<glm::vec3> normals;
			DynArray<glm::vec3> tangents;
			DynArray<glm::vec3> bitangents;
			
			DynArray<glm::vec2> texmap;
			DynArray<glm::vec4> vertexColors;
			
			DynArray<glm::uvec3> faces;
			DynArray<glm::uvec2> edges;
			DynArray<uint32_t> points; // @todo test
			
			uint32_t matIndex;
			
			String name;
			
		public:
			inline Mesh() = default;
			inline Mesh(Mesh const &) = default;
			inline Mesh(Mesh &&) = default;
			
			// msh must be triangulated
			Mesh(const aiMesh *msh):
				vertices(), normals(), tangents(), bitangents(),
				texmap(), vertexColors(),
				faces(), edges(), points(),
				matIndex(msh->mMaterialIndex), name(msh->mName.C_Str())
			{
				// don't accept cubemap textures
				CM3D_ASSERT(msh->mNumUVComponents[0] == 2 || !msh->mTextureCoords[0]);
				const uint32_t len = msh->mNumVertices;
				vertices.resize(len);
				
				if (msh->mNormals) normals.resize(len);
				if (msh->mTangents) tangents.resize(len);
				if (msh->mBitangents) bitangents.resize(len);
				
				if (msh->mTextureCoords[0]) texmap.resize(len);
				if (msh->mColors[0]) vertexColors.resize(len);
				
				// ======================================
				
				for (uint32_t i = 0; i < len; ++i) {
					vertices[i] = glm::vec3(
						msh->mVertices[i].x,
						msh->mVertices[i].y,
						msh->mVertices[i].z
					);
				}
				for (uint32_t i = 0; i < normals.size(); ++i) {
					normals[i] = glm::vec3(
						msh->mNormals[i].x,
						msh->mNormals[i].y,
						msh->mNormals[i].z
					);
				}
				for (uint32_t i = 0; i < tangents.size(); ++i) {
					tangents[i] = glm::vec3(
						msh->mTangents[i].x,
						msh->mTangents[i].y,
						msh->mTangents[i].z
					);
				}
				for (uint32_t i = 0; i < bitangents.size(); ++i) {
					bitangents[i] = glm::vec3(
						msh->mBitangents[i].x,
						msh->mBitangents[i].y,
						msh->mBitangents[i].z
					);
				}
				for (uint32_t i = 0; i < texmap.size(); ++i) {
					texmap[i] = glm::vec2(
						msh->mTextureCoords[0][i].x,
						msh->mTextureCoords[0][i].y
					);
				}
				for (uint32_t i = 0; i < vertexColors.size(); ++i) {
					vertexColors[i] = glm::vec4(
						msh->mColors[0][i].r,
						msh->mColors[0][i].g,
						msh->mColors[0][i].b,
						msh->mColors[0][i].a
					);
				}
				
				// =========================================
				
				for (uint32_t i = 0; i < msh->mNumFaces; ++i)
				{
					const auto &f = msh->mFaces[i];
					CM3D_ASSERT(f.mNumIndices < 4);
					
					if (f.mNumIndices == 3)
						faces.pushBack(glm::uvec3(
							f.mIndices[0],
							f.mIndices[1],
							f.mIndices[2]
						));
					else if (f.mNumIndices == 2)
						edges.pushBack(glm::uvec2(
							f.mIndices[0],
							f.mIndices[1]
						));
					else points.pushBack(f.mIndices[1]);
				}
			}
			
			constexpr inline const auto &getVertices() const { return vertices; }
			constexpr inline const auto &getNormals() const { return normals; }
			constexpr inline const auto &getTangents() const { return tangents; }
			constexpr inline const auto &getBitangents() const { return bitangents; }
			
			constexpr inline const auto &getTexmap() const { return texmap; }
			constexpr inline const auto &getVertexColors() const { return vertexColors; }
			
			constexpr inline const auto &getFaces() const { return faces; }
			constexpr inline const auto &getEdges() const { return edges; }
			constexpr inline const auto &getPoints() const { return points; }
			
			constexpr inline uint32_t getMatIndex() const { return matIndex; }
			constexpr inline const auto &getName() const { return name; }
		};
		
		// the node isn't standalone. Intended to have all existing meshes and materials
		// somewhere outside of it
		struct Node
		{
			uint32_t *meshesIdx = nullptr;
			Node *inner = nullptr;
			
			uint32_t numMeshes;
			uint32_t numInner;
			
			glm::mat4 transform;
			String name;
			
			inline ~Node() {
				if (meshesIdx)
					delete[] meshesIdx;
				if (inner)
					delete[] inner;
			}
			
			inline Node() = default;
			
			inline Node (Node const &oth):
				numMeshes(oth.numMeshes), numInner(oth.numInner),
				transform(oth.transform), name(oth.name)
			{
				meshesIdx = new uint32_t[numMeshes];
				for (uint32_t i = 0; i != numMeshes; ++i)
					meshesIdx[i] = oth.meshesIdx[i];
				
				inner = new Node[numInner];
				for (uint32_t i = 0; i != numInner; ++i)
					inner[i] = oth.inner[i];
			}
			
			inline Node (Node &&rv):
				meshesIdx(rv.meshesIdx), inner(rv.inner),
				numMeshes(rv.numMeshes), numInner(rv.numInner),
				transform(rv.transform), name(rv.name)
			{
				rv.meshesIdx = nullptr;
				rv.inner = nullptr;
			}
			
			inline Node &operator =(Node const &oth)
			{
				numMeshes = oth.numMeshes;
				numInner = oth.numInner;
				transform = oth.transform;
				name = oth.name;
				
				meshesIdx = new uint32_t[numMeshes];
				for (uint32_t i = 0; i != numMeshes; ++i)
					meshesIdx[i] = oth.meshesIdx[i];
				
				inner = new Node[numInner];
				for (uint32_t i = 0; i != numInner; ++i)
					inner[i] = oth.inner[i];
				
				return *this;
			}
			
			inline Node &operator =(Node &&rv)
			{
				meshesIdx = rv.meshesIdx;
				inner = rv.inner;
				rv.meshesIdx = nullptr;
				rv.inner = nullptr;
				
				numMeshes = rv.numMeshes;
				numInner = rv.numInner;
				transform = rv.transform;
				name = rv.name;
				
				return *this;
			}
		};
	}
}

#endif