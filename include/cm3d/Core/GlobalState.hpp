#ifndef CM3D_GLOBAL_STATE_HPP
#define CM3D_GLOBAL_STATE_HPP

#include <functional>

// @todo add function for adding log manager

namespace cm3d::gs
{
	size_t addShutdownTask(std::function<void()> fn);
	void removeShutdownTask(size_t id);
	void init();
	void shutdown();
}

#endif
