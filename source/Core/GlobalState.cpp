#include <cm3d/Core/GlobalState.hpp>

#include <cm3d/Tl/UniqueStorage.hpp>

static cm3d::UniqueStorage<std::function<void()>> shutdownTasks;

namespace cm3d::gs
{
	size_t addShutdownTask(std::function<void()> fn) {
		return shutdownTasks.push(fn);
	}

	void removeShutdownTask(size_t id) {
		shutdownTasks.erase(id);
	}

	void init() {}

	void shutdown() {
		for (auto &fn: shutdownTasks)
			fn();
		shutdownTasks.clear();
	}
}
