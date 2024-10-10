#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace cm3d::app
{
	inline double getTime() {
		return glfwGetTime();
	}
	inline void setTime(double time) {
		glfwSetTime(time);
	}
	inline void pollEvents() {
		glfwPollEvents();
	}
}
