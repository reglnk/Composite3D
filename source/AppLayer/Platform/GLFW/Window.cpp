#include <cm3d/AppLayer/Platform/GLFW/Window.hpp>

#include <memory>

namespace cm3d::app::glfw
{
	void mouseCallbackFn(GLFWwindow *w, int button, int action, int mods) {
		auto self = reinterpret_cast<Window *>(glfwGetWindowUserPointer(w));
		mouseEvent ev {
			self,
			button,
			action == GLFW_RELEASE ? Action::Release : Action::Press
		};
		for (auto const &cb: self->mouseCallbacks) {
			cb(ev);
		}
	}

	void keyCallbackFn(GLFWwindow *w, int key, int scancode, int action, int mods) {
		auto self = reinterpret_cast<Window *>(glfwGetWindowUserPointer(w));
		keyboardEvent ev {
			self,
			key,
			scancode,
			action == GLFW_RELEASE ? Action::Release : Action::Press
		};
		for (auto const &cb: self->keyCallbacks) {
			cb(ev);
		}
	}

	void offCallbackFn(GLFWwindow *w, double x, double y) {
		auto self = reinterpret_cast<Window *>(glfwGetWindowUserPointer(w));
		cursorOffsetEvent ev {self, x, y};
		for (auto const &cb: self->offCallbacks) {
			cb(ev);
		}
	}

	void resizeCallbackFn(GLFWwindow *w, int width, int height) {
		auto self = reinterpret_cast<Window *>(glfwGetWindowUserPointer(w));
		resizeEvent ev {self, width, height};
		for (auto const &cb: self->resizeCallbacks) {
			cb(ev);
		}
	}

	void Window::setupCallbacks() {
		glfwSetMouseButtonCallback(win, mouseCallbackFn);
		glfwSetKeyCallback(win, keyCallbackFn);
		glfwSetCursorPosCallback(win, offCallbackFn);
		glfwSetFramebufferSizeCallback(win, resizeCallbackFn);
	}

	void Window::addMouseCallback(mouseEventFn callback) {
		mouseCallbacks.push_back(callback);
	}

	void Window::addKeyCallback(keyboardEventFn callback) {
		keyCallbacks.push_back(callback);
	}

	void Window::addCursorOffsetCallback(cursorOffsetEventFn callback) {
		offCallbacks.push_back(callback);
	}

	void Window::addResizeCallback(resizeEventFn callback) {
		resizeCallbacks.push_back(callback);
	}
}
