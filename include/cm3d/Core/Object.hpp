#ifndef CM3D_OBJECT_BASE_HPP
#define CM3D_OBJECT_BASE_HPP

#include <cm3d/Modules/RenderSurface.hpp>

#include <map>
#include <cstdint>

namespace cm3d
{
	class Object
	{
	protected:
		const uint32_t id;
		std::map<uint32_t, void *> modules;
	public:
		inline Object(uint32_t id): id(id) {}
		inline ~Object() {
			for (auto &p: modules) {
				if (p.first == Modules::Id::RenderSurface) {
					delete reinterpret_cast<Modules::RenderSurface *>(p.second);
				}
				else CM3D_ASSERT(0 && "Invalid module ID");
			}
		}
		inline const auto &getModules() const {
			return modules;
		}
	};
}

#endif
