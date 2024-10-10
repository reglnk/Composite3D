#ifndef CM3D_APPLAYER_PLATFORM_GLFW_WINDOW_HPP
#define CM3D_APPLAYER_PLATFORM_GLFW_WINDOW_HPP

#include <cm3d/AppLayer/windowBase.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace cm3d::app::glfw
{
	class Window: public windowBase
	{
	protected:
		GLFWwindow *win = NULL;

		friend void mouseCallbackFn(GLFWwindow *w, int button, int action, int mods);
		friend void keyCallbackFn(GLFWwindow *w, int key, int scancode, int action, int mods);
		friend void offCallbackFn(GLFWwindow *w, double x, double y);
		friend void resizeCallbackFn(GLFWwindow *w, int width, int height);

		void setupCallbacks();

	public:
		Window() = default;

		Window(int width, int height, String const &title = "") {
			if (!create(width, height, title))
				throw std::runtime_error("GLFW: failed to create window");

			setupCallbacks();
		}

		Window(Window const &oth) = delete;

		Window(Window &&rv): win(rv.win) {
			glfwSetWindowUserPointer(win, this);
		}

		bool create(int width, int height, String const &title = "") override {
			win = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
			if (win == NULL)
				return false;

			// the key idea and flexibility source of the crossover interface around GLFW
			glfwSetWindowUserPointer(win, this);
			return true;
		}

		bool getKey(int code) override { return glfwGetKey(win, code); }
		bool shouldClose() override { return glfwWindowShouldClose(win); }
		void destroy() override { glfwDestroyWindow(win); }
		void makeCurrent() override { glfwMakeContextCurrent(win); }

		void addMouseCallback(mouseEventFn ev) override;
		void addKeyCallback(keyboardEventFn callback) override;

		void addCursorOffsetCallback(cursorOffsetEventFn callback) override;
		void addResizeCallback(resizeEventFn callback) override;

		void *getHandle() override { return win; }

		~Window() override {
			if (win != NULL)
				destroy();
		}
	};
}

#endif
