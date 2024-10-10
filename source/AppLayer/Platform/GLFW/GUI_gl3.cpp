#include <cm3d/AppLayer/GUI.hpp>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

namespace cm3d::app::gui
{
	void initRaw(windowBase &win) {
		win.makeCurrent();
		// auto cbkey {std::move(win.keyCallbacks)};
		// auto cbmou {std::move(win.mouseCallbacks)};
		// auto cboff {std::move(win.offCallbacks)};
		// @todo delete

		ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow *>(win.getHandle()), true);
		// win.add
		const char* glsl_version = "#version 330";
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	void newFrame() {
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
	}

	void renderFinal(ImDrawData *data) {
		ImGui_ImplOpenGL3_RenderDrawData(data);
	}

	void shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}
}
