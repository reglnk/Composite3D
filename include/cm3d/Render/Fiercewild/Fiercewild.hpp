#ifndef CM3D_RENDER_FIERCEWILD_HPP
#define CM3D_RENDER_FIERCEWILD_HPP

#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Tl/UniqueStorage.hpp>

#include <cm3d/Core/Object.hpp>
#include <cm3d/Core/World.hpp>
#include <cm3d/Core/FileSystem.hpp>
#include <cm3d/Core/LogManager.hpp>
#include <cm3d/Core/GlobalState.hpp>

#include <cm3d/Utility/GL/ShaderProgram.hpp>

#include <cm3d/Render/Base.hpp>

#include <glad/gl.h>
#include <map>

// OpenGL 3.3 Core renderer

namespace cm3d::Render::Fiercewild
{
	class Fiercewild: public Render::Base
	{
	public:
		FileSystem::sPath shadersDir;
		LogManager *logMgr;

	protected:
		struct objRenderData
		{
			DynArray<Object::pModule> modules;
			DynArray<Object::pModShared> sharedModules;
		};
		// Each object has corresponding set of graphics modules instances,
		// which can be accessed via object ID
		// @todo refactor
		std::map<std::size_t, objRenderData> mData;

		// the key is raw pointer from pModShared of render surface
		std::unordered_map<Modules::Base const *, Object::pModShared> mShared;
		float mFovAspect;

		int fnInit() override;
		int fnShutdown() override;
		int fnDisplay(glm::mat4 const &, Vector3 const *) override;

		// @todo function fnBindWorld that will set callbacks which will be called
		// when the object is added or updated in the world

		int fnLoad(World const &world, std::size_t id) override;
		int fnUnload(std::size_t id) override;

	public:
		void preconfigure() override;
		void syncFramebuffer(int width, int height) override;

		inline Fiercewild(FileSystem::sPath const &shaders_dir = ".", LogManager *mgr = nullptr)
		: shadersDir(shaders_dir)
		, logMgr(mgr)
		{
			gs::addShutdownTask(glfwTerminate);
		}

		inline ~Fiercewild() override {}

	protected:
		void rs_process_fn(std::size_t id, cm3d::Modules::Base const *m);
		void rsshared_process_fn(std::size_t id, cm3d::Modules::Base const *m);
	};
}

#endif
