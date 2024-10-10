#ifndef CM3D_RENDER_WINDOW_CONFIG_HPP
#define CM3D_RENDER_WINDOW_CONFIG_HPP

/* =======================================================
 * The window abstraction layer for passing settings
 * before creation and applying after/at creation of the window.
**/

#include <cm3d/Tl/String.hpp>
#include <cm3d/Tl/DynArray.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <cstdint>

namespace cm3d
{
	namespace Render
	{
		struct WinConfig
		{
			glm::ivec2 wSize;
			glm::ivec2 wPosition;
			
			String wTitle;
			String wHint;
			
			GLFWmonitor *wMonitor;
			GLFWwindow *wShare;
			
			inline WinConfig
			(
				glm::ivec2 size = glm::ivec2(0, 0), // x, y <= 0 : auto
				glm::ivec2 position = glm::ivec2(0, 0),
				String const &title = "",
				String const &hint = "",
				GLFWmonitor *monitor = nullptr,
				GLFWwindow *share = nullptr
			):
				wSize(size), wPosition(position),
				wTitle(title), wHint(hint),
				wMonitor(monitor), wShare(share)
			{}
		};
	}
}

#endif
