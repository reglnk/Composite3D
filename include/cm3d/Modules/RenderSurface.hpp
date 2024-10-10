#ifndef CM3D_OBJECT_MODULE_RENDER_SURFACE_HPP
#define CM3D_OBJECT_MODULE_RENDER_SURFACE_HPP

/* Representation of an object surface that can be rendered:
 * triangles, lines, materials, textures etc. Independent from
 * any graphics API. */

#include <cm3d/Modules/Base.hpp>
#include <cm3d/Modules/Position.hpp> // runtime dependency

#include <cm3d/Tl/Aliases.hpp>
#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Tl/Vector3.hpp>
#include <cm3d/Core/Graphics.hpp>
#include <cm3d/Core/FileSystem.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>

#include <cstdint>
#include <map>
#include <string>

namespace cm3d::Modules
{
	class RenderSurface: public Base
	{
	public:
		static constexpr uint32_t Id = 1;
		inline uint32_t getId() const override {
			return Id;
		}
		RenderSurface() noexcept = default;
		
	protected:
		FileSystem::sPath loadPath;
		
		Graphics::Node rootNode;
		DynArray<Graphics::Mesh> meshes;
		DynArray<Graphics::Material> materials;
		DynArray<Graphics::Texture2D> textures;
	
	public:
		// @todo glm::mat4 offset; that will offset the RenderSurface from the object's physical position

		const auto &getPath() const { return loadPath; }
		const auto &getRoot() const { return rootNode; }
		const auto &getMeshes() const { return meshes; }
		const auto &getMaterials() const { return materials; }
		const auto &getTextures() const { return textures; }
	
	protected:
		void copyAiMaterial (
			aiMaterial const *mtl,
			Graphics::Material *gmtl
		) {
			gmtl->name = String(mtl->GetName().C_Str());

			ai_real prop;
			mtl->Get(AI_MATKEY_METALLIC_FACTOR, prop);  gmtl->metalness = prop;
			mtl->Get(AI_MATKEY_ROUGHNESS_FACTOR, prop); gmtl->roughness = prop;
			mtl->Get(AI_MATKEY_OPACITY, prop);          gmtl->opacity = prop;
			
			aiColor3D col;
			mtl->Get(AI_MATKEY_COLOR_DIFFUSE, col);     gmtl->diffuse = glm::vec3(col.r, col.g, col.b);
			mtl->Get(AI_MATKEY_COLOR_SPECULAR, col);    gmtl->specular = glm::vec3(col.r, col.g, col.b);
			mtl->Get(AI_MATKEY_COLOR_AMBIENT, col);     gmtl->ambient = glm::vec3(col.r, col.g, col.b);
			mtl->Get(AI_MATKEY_COLOR_TRANSPARENT, col); gmtl->transparent = glm::vec3(col.r, col.g, col.b);
			mtl->Get(AI_MATKEY_COLOR_EMISSIVE, col);    gmtl->emissive = glm::vec3(col.r, col.g, col.b);
			
			const auto addTex = [this, mtl, gmtl](aiTextureType type)
			{
				unsigned int texCount = mtl->GetTextureCount(type);
				for (unsigned int i = 0; i != texCount; ++i)
				{
					aiString texpath;
					mtl->GetTexture(type, i, &texpath);
					Graphics::Texture2D newTex(type, texpath.C_Str());
					
					gmtl->texIndex.pushBack(textures.length());
					this->textures.pushBack(std::move(newTex));
				}
			};
			
			addTex(aiTextureType_DIFFUSE);
			addTex(aiTextureType_SPECULAR);
			addTex(aiTextureType_AMBIENT);
			addTex(aiTextureType_EMISSIVE);
			
			addTex(aiTextureType_NORMALS);
			addTex(aiTextureType_DISPLACEMENT);
			
			// PBR textures
			addTex(aiTextureType_METALNESS);
			addTex(aiTextureType_AMBIENT_OCCLUSION);
		}
		
		// Copy assimp node recursively to Graphics::Node. The structure is similar
		static void copyAiNode(
			aiScene const *scene,
			aiNode const *src,
			Graphics::Node *dst,
			DynArray<Graphics::Mesh> *meshes
		) {
			const aiMatrix4x4 &mt = src->mTransformation;
			dst->transform = glm::mat4(
				glm::vec4(mt.a1, mt.a2, mt.a3, mt.a4),
				glm::vec4(mt.b1, mt.b2, mt.b3, mt.b4),
				glm::vec4(mt.c1, mt.c2, mt.c3, mt.c4),
				glm::vec4(mt.d1, mt.d2, mt.d3, mt.d4)
			);
			dst->name = src->mName.C_Str();
			
			dst->numMeshes = src->mNumMeshes;
			dst->meshesIdx = new uint32_t[src->mNumMeshes];
			meshes->reserve(src->mNumMeshes);
			
			for (uint32_t i = 0; i < src->mNumMeshes; ++i)
			{
				dst->meshesIdx[i] = meshes->size();
				meshes->pushBack(Graphics::Mesh(scene->mMeshes[src->mMeshes[i]]));
			}
			
			dst->numInner = src->mNumChildren;
			dst->inner = new Graphics::Node[src->mNumChildren];
			for (uint32_t i = 0; i < src->mNumChildren; ++i)
				copyAiNode(scene, src->mChildren[i], &dst->inner[i], meshes);
		}
		
	public:
		inline ~RenderSurface() override = default;
		
		// Load the scene using Assimp
		// @todo move to /source
		int load(FileSystem::sPath const &path, String *output = nullptr)
		{
			CM3D_ASSERT(FileSystem::isNormalized(path));
			loadPath = path;
			
			Assimp::Importer importer;
			const struct aiScene* scene = importer.ReadFile(path.c_str(),
				aiProcess_Triangulate | aiProcess_GenNormals |
				aiProcess_CalcTangentSpace | aiProcess_FindDegenerates |
				aiProcess_ValidateDataStructure | aiProcess_FindInvalidData
			);
			
			if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
				if (output) *output += importer.GetErrorString();
				return 1;
			}
			
			rootNode = Graphics::Node();
			meshes.clear();
			meshes.reserve(scene->mNumMeshes);
			materials.resize(scene->mNumMaterials);
			textures.clear();
			
			for (unsigned int i = 0; i != scene->mNumMaterials; ++i)
				copyAiMaterial(scene->mMaterials[i], &materials[i]);
			
			for (auto &tex: textures)
			{
				using namespace FileSystem;
				
				if (!isAbsolute(tex.path))
					tex.path = FileSystem::concat(parentDir(path), tex.path);
				
				tex.load();
				if (!tex) {
					if (output) *output += String("cm3d::Modules::RenderSurface::load: failed to load texture ") + tex.path + "\n";
					return 2;
				}
			}
			copyAiNode(scene, scene->mRootNode, &rootNode, &meshes);
			return 0;
		}
		
		inline RenderSurface &operator =(RenderSurface const &oth) = default;
		inline RenderSurface &operator =(RenderSurface &&rv) = default;
	};
}

#endif
