#ifndef CM3D_OBJECT_BASE_HPP
#define CM3D_OBJECT_BASE_HPP

#include <cm3d/Tl/UniqueStorage.hpp>
#include <cm3d/Modules/Base.hpp>

#include <bitsery/bitsery.h>

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace cm3d
{
	class Object
	{
	public:
		using pModule = std::unique_ptr<Modules::Base>;
		using pModShared = std::shared_ptr<Modules::Base>;

	protected:
		UniqueStorage<pModule> modules;
		UniqueStorage<pModShared> sharedModules;
		std::unordered_multimap<uint32_t, size_t> idMap;
		std::unordered_multimap<uint32_t, size_t> idMapShared;

	public:
		inline Object() = default;

		constexpr inline const auto &getModules() const {
			return modules;
		}
		constexpr inline const auto &getSharedModules() const {
			return sharedModules;
		}

		inline pModule &operator [](size_t localId) {
			return modules[localId];
		}
		inline pModule const &operator [](size_t localId) const {
			return modules[localId];
		}
		inline size_t addModule(Modules::Base *m) {
			size_t localId = modules.push(pModule(m));
			idMap.insert({m->getId(), localId});
			return localId;
		}
		inline Modules::Base *detachModule(size_t localId)
		{
			pModule &m = modules[localId];
			uint32_t mId = m->getId();
			Modules::Base *pmod = m.release();
			modules.erase(localId);

			for (auto iter = idMap.find(mId); iter != idMap.end(); ++iter) {
				if (iter->second == localId) {
					idMap.erase(iter);
					return pmod;
				}
			}
			throw pmod;
		}
		inline void delModule(size_t localId) {
			delete detachModule(localId);
		}

		inline pModShared &getShared(size_t localId) {
			return sharedModules[localId];
		}
		inline pModule const &getShared(size_t localId) const {
			return modules[localId];
		}
		inline size_t addShared(pModShared m) {
			uint32_t mId = m->getId();
			size_t localId = sharedModules.push(std::move(m));
			idMapShared.insert({mId, localId});
			return localId;
		}
		inline pModShared detachShared(size_t localId)
		{
			pModShared m = sharedModules[localId];
			uint32_t mId = m->getId();
			sharedModules.erase(localId);

			for (auto iter = idMapShared.find(mId); iter != idMapShared.end(); ++iter) {
				if (iter->second == localId) {
					idMapShared.erase(iter);
					return m;
				}
			}
			throw m;
		}

	public:
		inline size_t operator +=(Modules::Base *m) {
			return addModule(m);
		}
		inline void operator -=(size_t localId) {
			delModule(localId);
		}
		inline Modules::Base *operator -(size_t localId) {
			return detachModule(localId);
		}

		inline size_t operator <<(pModShared m) {
			return addShared(std::move(m));
		}
		inline pModShared operator >>(size_t localId) {
			return detachShared(localId);
		}

	public:
		template<typename TOutputAdapter, typename TContext>
		void serialize(bitsery::Serializer<TOutputAdapter, TContext>& s) const {
			s.object(modules);
		}

		template<typename TInputAdapter, typename TContext>
		void serialize(bitsery::Deserializer<TInputAdapter, TContext>& s) {
			s.object(modules);
		}
	};
}

/*
 * inline void addModule(Modules::Base *m) {
			modules.insert({m->getId(), pModule(m)});
		}
		inline Object &operator <<(Modules::Base *m) {
			addModule(m);
			return *this;
		}
		inline void addSharedModule(pModShared m) {
			sharedModules.insert({m->getId(), m});
		}
		inline Object &operator >>(pModShared m) {
			addSharedModule(m);
			return *this;
		}
		inline Modules::Base *detachModule(std::size_t index) {
			CM3D_ASSERT(modules[index]);
			auto ptr = modules[index].release();
			modules.erase(index);
			return ptr;
		}
		inline Object &operator <<=(std::size_t index) {
			CM3D_ASSERT(modules[index]);
			modules.erase(index);
			return *this;
		}
		inline pModShared detachSharedModule(std::size_t index) {
			pModShared m = sharedModules[index];
			CM3D_ASSERT(m);
			sharedModules.erase(index);
			return m;
		}
		inline Object &operator >>=(std::size_t index) {
			CM3D_ASSERT(sharedModules[index]);
			sharedModules.erase(index);
			return *this;
		}
		inline void delModule(std::size_t index) {
			CM3D_ASSERT(modules[index]);
			modules.erase(index);
		}
		inline Object &operator -=(std::size_t index) {
			delModule(index);
			return *this;
		}
 * */

#endif
