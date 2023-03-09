#ifndef CM3D_OBJECT_MODULE_RENDER_SURFACE_HPP
#define CM3D_OBJECT_MODULE_RENDER_SURFACE_HPP

#include <Modules/Base.hpp>

#include <Types/Aliases.hpp>
#include <Types/DynArray.hpp>
#include <Types/Vector3.hpp>
#include <Core/Graphics.hpp>
#include <Core/FileSystem.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>

#include <cstdint>
#include <map>
#include <string>

namespace cm3d
{
	namespace Modules
	{
		namespace Id {
			constexpr uint32_t RenderSurface = 0x1001;
		}
		
		class RenderSurface: public Base
		{
		protected:
			FileSystem::sPath loadPath;
			
			Graphics::Node rootNode;
			DynArray<Graphics::Mesh> meshes;
			DynArray<Graphics::Material> materials;
			DynArray<Graphics::Texture2D> textures;
		
		public:
			Vector3 position = Vector3(0.0);
			
			inline const auto &getPath() const { return loadPath; }
			inline const auto &getRoot() const { return rootNode; }
			inline const auto &getMeshes() const { return meshes; }
			inline const auto &getMaterials() const { return materials; }
			inline const auto &getTextures() const { return textures; }
		
		protected:
			void copyAiMaterial(
				aiMaterial const *mtl,
				Graphics::Material *gmtl
			) {
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
						aiString name;
						mtl->GetTexture(type, i, &name);
						Graphics::Texture2D newTex = {type, name.C_Str()};
						
						this->textures.pushBack(newTex);
						// add link to the last added texture
						gmtl->textures.pushBack(textures.rBegin());
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
				for (uint32_t i = 0; i < src->mNumMeshes; ++i)
				{
					dst->meshesIdx[i] = src->mMeshes[i];
					CM3D_ASSERT(src->mMeshes[i] == meshes->size());
					meshes->pushBack(scene->mMeshes[src->mMeshes[i]]);
				}
				
				dst->numInner = src->mNumChildren;
				dst->inner = new Graphics::Node[src->mNumChildren];
				for (uint32_t i = 0; i < src->mNumChildren; ++i)
					copyAiNode(scene, src->mChildren[i], &dst->inner[i], meshes);
			}
			
		public:
			inline RenderSurface(): Base(Id::RenderSurface) {}
			
			// Load the scene using Assimp
			// @todo move to /source
			int load(FileSystem::sPath const &path, String *output)
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
					*output += importer.GetErrorString();
					return 1;
				}
				
				rootNode = Graphics::Node();
				meshes.clear();
				meshes.reserve(scene->mNumMeshes);
				materials.resize(scene->mNumMaterials);
				textures.clear();
				
				for (unsigned int i = 0; i != scene->mNumMaterials; ++i) {
					copyAiMaterial(scene->mMaterials[i], materials.begin() + i);
				}
				for (auto &tex: textures) {
					if (!tex) {
						tex.path = FileSystem::concat(loadPath, tex.path);
						tex.load();
						if (!tex) {
							*output += String("RenderSurface::load: failed to load texture ") + tex.path + "\n";
							return 2;
						}
					}
				}
				copyAiNode(scene, scene->mRootNode, &rootNode, &meshes);
				return 0;
			}
		};
	}
}

#endif
