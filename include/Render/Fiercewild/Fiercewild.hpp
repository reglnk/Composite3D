#ifndef CM3D_RENDER_FIERCEWILD_HPP
#define CM3D_RENDER_FIERCEWILD_HPP

#include <Types/DynArray.hpp>
#include <Types/UniqueStorage.hpp>

#include <Core/Object.hpp>
#include <Core/World.hpp>

#include <Render/Base.hpp>

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
