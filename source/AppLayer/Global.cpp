#include <cm3d/AppLayer/Global.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

namespace cm3d::app
{
	void winSystemInit()
	{
		if (glfwInit() != GLFW_TRUE)
		{
			const char *desc;
			glfwGetError(&desc);
			std::string message("glfwInit error");
			if (desc != NULL)
				message += desc;
			throw std::runtime_error(message);
		}
	}
	void winSystemShutdown()
	{
		glfwTerminate();
	}
}
