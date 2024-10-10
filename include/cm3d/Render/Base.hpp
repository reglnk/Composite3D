#ifndef CM3D_RENDER_BASE_HPP
#define CM3D_RENDER_BASE_HPP

#include <cm3d/Core/Object.hpp>
#include <cm3d/Core/World.hpp>
#include <cm3d/Core/FPCamera.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <stdexcept>
#include <string>

namespace cm3d::Render
{
	class Base
	{
	public:
		std::function<void(int, const char *)> onErrorIgnore = [](int retcode, const char *method) {
			throw std::runtime_error(std::string("Renderer error: ") + method + " - " + std::to_string(retcode));
		};

	protected:
		GLFWwindow *mWindow = nullptr;
		World *mWorld = nullptr;

		// Initialize graphics API if needed
		virtual int fnInit() { return 0; }
		virtual int fnShutdown() { return 0; }

		// Draw one image into mWindow
		virtual int fnDisplay(glm::mat4 const &, Vector3 const *) { return 0; }

		// Load data for nothing more but rendering, from modules like RenderSurface
		virtual int fnLoad(World const &, std::size_t) { return 0; }
		virtual int fnUnload(std::size_t) { return 0; }

	public:
		constexpr inline GLFWwindow *getWindow() {
			return mWindow;
		}
		virtual void bindWindow(GLFWwindow *wnd) {
			mWindow = wnd;
		}
		virtual void bindWorld(World &w) {
			mWorld = &w;
		}

		// send some glfwWindowHint's
		virtual void preconfigure() {}

		virtual void syncFramebuffer(int width, int height) {}

		void initialize(int *result = nullptr) {
			int res = fnInit();
			if (result)
				*result = res;
			else if (res != 0)
				onErrorIgnore(res, "initialize");
		}
		void shutdown(int *result = nullptr) {
			int res = fnShutdown();
			if (result)
				*result = res;
			else if (res != 0)
				onErrorIgnore(res, "shutdown");
		}
		void display(glm::mat4 const &view, Vector3 const *position = nullptr, int *result = nullptr) {
			int res = fnDisplay(view, position);
			if (result)
				*result = res;
			else if (res != 0)
				onErrorIgnore(res, "display");
		}

		void loadObject(World const &world, std::size_t id, int *result = nullptr) {
			int res = fnLoad(world, id);
			if (result)
				*result = res;
			else if (res != 0)
				onErrorIgnore(res, "loadObject");
		}
		void unloadObject(std::size_t id, int *result = nullptr) {
			int res = fnUnload(id);
			if (result)
				*result = res;
			else if (res != 0)
				onErrorIgnore(res, "unloadObject");
		}

		virtual ~Base() = default;
	};
}

// for reference
// @see ./Fiercewild/Fiercewild.hpp
// @see ./Fiercewild/Fiercewild.cpp

#endif
