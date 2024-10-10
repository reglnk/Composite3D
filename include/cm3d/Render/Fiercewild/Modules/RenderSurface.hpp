#ifndef CM3D_RENDER_FIERCEWILD_MODULE_RENDER_SURFACE_HPP
#define CM3D_RENDER_FIERCEWILD_MODULE_RENDER_SURFACE_HPP

#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Tl/UniqueStorage.hpp>

#include <cm3d/Core/Object.hpp>
#include <cm3d/Core/World.hpp>
#include <cm3d/Core/FileSystem.hpp>
#include <cm3d/Utility/GL/ShaderProgram.hpp>

#include <cm3d/Modules/RenderSurface.hpp> 
#include <cm3d/Render/Base.hpp>

#include <glad/gl.h>

#include <iostream>

namespace cm3d::Render::Fiercewild
{
	struct MeshRenderData
	{
		GLuint VAO = 0;
		GLuint EBO = 0;
		
		// Fragment shader input
		// 0. vec3 vertices
		// 1. vec2 texcoords
		// 2. vec3 normals
		// 3. vec3 tangents
		// 4. vec3 bitangents
		// 5. vec4 vertcolors
		GLuint VBO[6] = {0, 0, 0, 0, 0, 0};
		
		uint32_t matIndex;
		uint32_t facesCount;
		
		inline MeshRenderData() = default;
		MeshRenderData(MeshRenderData const &) = delete;
		MeshRenderData(MeshRenderData &&rv): VAO(rv.VAO), EBO(rv.EBO), matIndex(rv.matIndex), facesCount(rv.facesCount)
		{
			rv.VAO = 0;
			rv.EBO = 0;
			std::memcpy(VBO, rv.VBO, sizeof(VBO));
			std::memset(rv.VBO, 0, sizeof(rv.VBO));
		}
		
		MeshRenderData(Graphics::Mesh const &msh);
		
		inline ~MeshRenderData()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &EBO);
			glDeleteBuffers(6, VBO);
		}
	};
	
	// RenderSurface data node
	struct rsdNode
	{
		// index of MeshRenderData
		uint32_t *meshesIdx = nullptr;
		rsdNode *inner = nullptr;
		
		uint32_t numMeshes;
		uint32_t numInner;
		
		glm::mat4 transform;
		
		inline ~rsdNode() {
			if (meshesIdx)
				delete[] meshesIdx;
			if (inner)
				delete[] inner;
		}
		
		inline rsdNode() = default;
		
		inline rsdNode(rsdNode const &) = delete;
		inline rsdNode(rsdNode &&rv):
			meshesIdx(rv.meshesIdx), inner(rv.inner),
			numMeshes(rv.numMeshes), numInner(rv.numInner),
			transform(rv.transform)
		{
			rv.meshesIdx = nullptr;
			rv.inner = nullptr;
		}
	};
	
	namespace Modules
	{
		class RenderSurface: public cm3d::Modules::Base
		{
		public:
			// the class is meant not to be stored along with regular modules
			// so it's possible to use ones' IDs to index objects of classes like this
			static constexpr uint32_t Id = 1;
			inline uint32_t getId() const override {
				return Id;
			}
		
			cm3d::Modules::RenderSurface const *rSurf;
			rsdNode rootNode; // follows rs->rootNode
			DynArray<MeshRenderData> meshesRd; // follows rs->meshes
			DynArray<Utility::GL::ShaderProgram *> matShaders; // follows rs->materials
			DynArray<GLuint> textures;
			
		protected:
			void copyRsNode (
				Graphics::Node const &src,
				rsdNode *dst
			) {
				dst->transform = src.transform;
				
				dst->numMeshes = src.numMeshes;
				dst->numInner = src.numInner;
				
				dst->meshesIdx = new uint32_t[src.numMeshes];
				dst->inner = new rsdNode[src.numInner];
				
				auto const &rsurf = *rSurf;
				for (uint32_t i = 0u; i != src.numMeshes; ++i)
					meshesRd.pushBack(MeshRenderData(rsurf.getMeshes()[src.meshesIdx[i]]));
				
				for (uint32_t i = 0u; i != src.numInner; ++i)
					copyRsNode(src.inner[i], &dst->inner[i]);
			}
			
			void loadTextures()
			{
				glGenTextures(textures.length(), textures.data());
				for (size_t i = 0; i != textures.length(); ++i)
				{
					Graphics::Texture2D const &tex = rSurf->getTextures()[i];
					GLenum format;
					
					if (tex.channels() == 3)
						format = GL_RGB;
					else if (tex.channels() == 4)
						format = GL_RGBA;
					else if (tex.channels() == 1)
						format = GL_RED;
					else {
						glDeleteTextures(1, &textures[i]);
						textures[i] = 0; // @todo check this
						continue;
					}
					
					glBindTexture(GL_TEXTURE_2D, textures[i]);
					glTexImage2D(GL_TEXTURE_2D, 0, format, tex.width(), tex.height(), 0, format, GL_UNSIGNED_BYTE, tex.data());
					
					// @todo extended texture configuration support
					glGenerateMipmap(GL_TEXTURE_2D);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
			}
			
			Utility::GL::ShaderProgram *createShader (
				Graphics::Material const &mat,
				FileSystem::sPath const &shaders_dir,
				String *output = nullptr
			);
			
		public:
			RenderSurface (
				cm3d::Modules::RenderSurface const *rs,
				FileSystem::sPath const &shaders_dir,
				String *output = nullptr
			);
			
			inline ~RenderSurface() override {
				for (auto shprog: matShaders)
					delete shprog;
			}
		};
	}
}

#endif
