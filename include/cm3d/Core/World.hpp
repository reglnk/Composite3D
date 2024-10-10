#ifndef CM3D_WORLD_HPP
#define CM3D_WORLD_HPP

#include <cm3d/Tl/UniqueStorage.hpp>
#include <cm3d/Core/FileSystem.hpp>
#include <cm3d/Core/Object.hpp>
#include <cm3d/Modules/RenderSurface.hpp>

#include <cstdio>
#include <limits>

namespace cm3d
{
	class World
	{
	public:
		struct Metadata
		{
			String name; // the human-readable world name
			// String desc;
		};
	protected:
		UniqueStorage<std::shared_ptr<Modules::Base>> sharedModules;
		
	public:
		UniqueStorage<Object> objects;
		Metadata meta;
		
		void addSharedModule(std::shared_ptr<Modules::Base> const &m) {
			sharedModules.push(m);
		}
		
		template<typename Module>
		std::shared_ptr<Module> addSharedModule() {
			auto shmod = std::make_shared<Module>();
			sharedModules.push(shmod);
			return shmod;
		}

		enum IOResult {
			Success = 0,
			FSError = 1,
			FormatError = 2,
			MemError = 3,
			NotSupported = 4
		};

		IOResult load(FileSystem::sPath const &file);
		IOResult save(FileSystem::sPath const &file, Metadata const &meta);
	};
}

#endif
