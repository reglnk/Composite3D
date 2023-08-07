#ifndef CM3D_RENDER_FIERCEWILD_HPP
#define CM3D_RENDER_FIERCEWILD_HPP

#include <cm3d/Types/DynArray.hpp>
#include <cm3d/Types/UniqueStorage.hpp>

#include <cm3d/Core/Object.hpp>
#include <cm3d/Core/World.hpp>

#include <cm3d/Render/Base.hpp>

#include <glad/gl.h>

#include <map>

namespace cm3d::Fiercewild
{
	class Renderer: public Render::Base
	{
	public:
		int pushEvent(Event ev) override;
		int pushObjEvent(objLocationType tp, ObjEvent ev) override;
		void framebufferSizeCallback(int width, int height) override;
	};
}

#endif
