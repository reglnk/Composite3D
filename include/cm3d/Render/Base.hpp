#ifndef CM3D_RENDER_BASE_HPP
#define CM3D_RENDER_BASE_HPP

// @todo: support several windows

#include <cm3d/Render/WinConfig.hpp>
#include <cm3d/Core/Object.hpp>
#include <cm3d/Types/UniqueStorage.hpp>

#include <GLFW/glfw3.h>

namespace cm3d
{
	namespace Render
	{
		class Base
		{
		protected:
			WinConfig *mConf = nullptr;
			GLFWwindow *mWindow = nullptr;
			
			// Each object has corresponding set of graphics modules instances,
			// which can be accessed via object ID
			std::map<const UniqueStorage<Object> *,
				UniqueStorage<DynArray<cm3d::Modules::Base>>> mData;
			
			typedef struct {
				const UniqueStorage<Object> *st;
				uint32_t id;
			} objLocationType;
			
		public:
			enum class Event
			{
				Init, // create window & load graphics API
				Shutdown, // unload graphics API
				Display,
				
				// DisplayAsync
				// DisplayJoin
				// DisplayFlush
			};
			enum class ObjEvent
			{
				Init,
				Reload,
				Unload,
				Update,
				Show,
				Hide
			};
			
			inline Base() = default;
			inline Base(WinConfig *conf): mConf(conf) {}
			
			inline void configure(WinConfig *conf) {
				mConf = conf;
			}
			inline GLFWwindow *getWindow() {
				return mWindow;
			}
			
			virtual int pushEvent(Event ev);
			virtual int pushObjEvent(objLocationType tp, ObjEvent ev);
			
			virtual void framebufferSizeCallback(int width, int height);
		};
	}
}

#endif
