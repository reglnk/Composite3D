#ifndef CM3D_RENDER_FIERCEWILD_MODULE_RENDER_SURFACE_HPP
#define CM3D_RENDER_FIERCEWILD_MODULE_RENDER_SURFACE_HPP

#include <cm3d/Types/DynArray.hpp>
#include <cm3d/Types/UniqueStorage.hpp>
#include <cm3d/Core/Object.hpp>
#include <cm3d/Core/World.hpp>
#include <cm3d/Modules/RenderSurface.hpp>
#include <cm3d/Render/Base.hpp>

#include <glad/gl.h>


namespace cm3d::Fiercewild
{
	struct MeshRenderData
	{
		GLuint VAO;
		GLuint EBO;
		
		// Fragment shader input
		// 0. vec3 vertices
		// 1. vec2 texcoords
		// 2. vec3 normals
		// 3. vec3 tangents
		// 4. vec3 bitangents
		// 5. vec4 vertcolors
		GLuint VBO[6];
	};
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
	};
	namespace Modules
	{
		// Classes defined in this namespace will follow the structure
		// of ones defined in cm3d::Modules
		class RenderSurface: public cm3d::Modules::Base
		{
		protected:
			DynArray<MeshRenderData> mrd;
			rsdNode root;
		public:
			inline RenderSurface(): Base(cm3d::Modules::Id::RenderSurface), cm3d::Modules::Base::mData() {}
		};
	}
}

#endif
