#ifndef CM3D_APPLAYER_GUI_HPP
#define CM3D_APPLAYER_GUI_HPP

#include <cm3d/Core/GlobalState.hpp>
#include <cm3d/AppLayer/windowBase.hpp>

#include <imgui.h>

namespace cm3d::app::gui
{
	extern void initRaw(windowBase &win);
	extern void newFrame();
	extern void renderFinal(ImDrawData *data);
	extern void shutdown();

	inline void init(windowBase &win) {
		initRaw(win);
		gs::addShutdownTask(shutdown);
	}
}

#endif
