#include <Render/Fiercewild/Fiercewild.hpp>

namespace cm3d
{
	namespace Render
	{
		int Fiercewild::pushEvent(Event ev)
		{
			if (ev == Event::Init)
			{
				GLFWmonitor *monitor = mConf && mConf->wMonitor ? mConf->wMonitor : glfwGetPrimaryMonitor();
				const GLFWvidmode* vm = glfwGetVideoMode(monitor);
				
				int winW = 0, winH = 0;
				if (mConf) {
					if (mConf->wSize.x > 0)
						winW = mConf->wSize.x;
					if (mConf->wSize.y > 0)
						winW = mConf->wSize.y;
				}
				if (!winW || !winH)
					glfwGetMonitorWorkarea(monitor, NULL, NULL, winW ? NULL : &winW, winH ? NULL : &winH);
				
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				
				glfwWindowHint(GLFW_ACCUM_RED_BITS, vm->redBits);
				glfwWindowHint(GLFW_ACCUM_GREEN_BITS, vm->greenBits);
				glfwWindowHint(GLFW_ACCUM_BLUE_BITS, vm->blueBits);
				glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, 8);
				glfwWindowHint(GLFW_REFRESH_RATE, vm->refreshRate);

				mWindow = glfwCreateWindow(winW, winH, "SBView 0.1.0 alpha", NULL, NULL);
				if (!mWindow)
					return 101;
				
				// glfwMakeContextCurrent(mWindow);
				if (!gladLoaderLoadGL())
					return 102;
			}
			else if (ev == Event::Shutdown) {
				gladLoaderUnloadGL();
			}
			else if (ev == Event::Display) {
				;
			}
			return 0;
		}
		static int initProcessNode(Graphics::Node const *node) {
			
		}
		static int initRenderSurface(Object const &obj) {
			auto rs = reinterpret_cast<Modules::RenderSurface const *>(
				obj.getModules().at(Modules::Id::RenderSurface)
			);
			initProcessNode(rs->rootNode);
		}
		int Fiercewild::pushObjEvent(Object const &obj, ObjEvent ev)
		{
			if (ev == ObjEvent::Init)
			{
				auto modules = obj.getModules();
				if (modules.count(Modules::Id::RenderSurface) == 0)
					return 1;
				
			}
		}
	}
}