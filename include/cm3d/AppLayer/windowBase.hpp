#ifndef CM3D_APPLAYER_WINDOW_BASE_HPP
#define CM3D_APPLAYER_WINDOW_BASE_HPP

#include <cm3d/AppLayer/Common.hpp>
#include <cm3d/Tl/String.hpp>

#include <functional>
#include <deque>
#include <vector>

namespace cm3d::app
{
	class windowBase;

	struct mouseEvent
	{
		windowBase *window;
		int button;
		Action action;
	};

	struct keyboardEvent
	{
		windowBase *window;
		int key;
		int scancode;
		Action action;
	};

	struct cursorOffsetEvent
	{
		windowBase *window;
		double x;
		double y;
	};

	struct resizeEvent
	{
		windowBase *window;
		int width;
		int height;
	};

	typedef std::function<void(mouseEvent &)> mouseEventFn;
	typedef std::function<void(keyboardEvent &)> keyboardEventFn;
	typedef std::function<void(cursorOffsetEvent &)> cursorOffsetEventFn;
	typedef std::function<void(resizeEvent &)> resizeEventFn;

	class windowBase
	{
	protected:
		windowBase() = default;

	public:
		void *userPtr;
		std::deque<std::function<void(windowBase *)>> shutdownTasks;

		std::vector<mouseEventFn> mouseCallbacks;
		std::vector<keyboardEventFn> keyCallbacks;
		std::vector<cursorOffsetEventFn> offCallbacks;
		std::vector<resizeEventFn> resizeCallbacks;

		virtual bool create(int width, int height, String const &title = "") { throw nullptr; }
		virtual bool getKey(int code) { throw nullptr; }
		virtual bool shouldClose() { throw nullptr; }
		virtual void destroy() { throw nullptr; }
		virtual void makeCurrent() { throw nullptr; }

		virtual void *getHandle() { throw nullptr; }

		virtual void addMouseCallback(mouseEventFn callback) { throw nullptr; }
		virtual void addKeyCallback(keyboardEventFn callback) { throw nullptr; }
		virtual void addCursorOffsetCallback(cursorOffsetEventFn callback) { throw nullptr; }
		virtual void addResizeCallback(resizeEventFn callback) { throw nullptr; }

		virtual ~windowBase() {
			for (auto &task: shutdownTasks)
				task(this);
		}
	};
}

#endif
