#ifndef CM3D_APPLAYER_GLOBAL_HPP
#define CM3D_APPLAYER_GLOBAL_HPP

#include <cm3d/Core/GlobalState.hpp>

namespace cm3d::app
{
	void winSystemInit();
	void winSystemShutdown();

	inline void winSystem() {
		winSystemInit();
		gs::addShutdownTask(winSystemShutdown);
	}

	double getTime();
	void setTime(double time);

	void pollEvents();
}

#include "Platform/GLFW/Global.inl"

#endif
