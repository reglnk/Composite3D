#ifndef CM3D_TYPE_UNIQUE_STORAGE_HPP
#define CM3D_TYPE_UNIQUE_STORAGE_HPP

#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Tl/Aliases.hpp>
#include <cm3d/Serialization/Types/SmartPtr.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/details/serialization_common.h>
#include <bitsery/ext/std_smart_ptr.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/brief_syntax.h>

#include <cstdint>
#include <cstring>
#include <cstddef>

#include <vector>
#include <type_traits>
#include <memory>
#include <limits>
#include <stdexcept>

// The container that maps items to IDs. Intended for usage with cm3d::Object
// @todo:
// specialize by T = Object and sort objects by position in their physics modules (if any)
// or find another way to optimize the objects interaction in 3D space (ECS for example)

namespace cm3d
{
	// Stores elements by IDs
	template<typename T>
	class UniqueStorage
	{
	public:
		using valueType = T;
		using indexType = std::size_t;
		
	protected:
		std::vector<T> mArr;
		// contains info about existence of each element of previous array
		std::vector<bool> mActive;
		// IDs of elements being deleted (acts as a stack)
		std::vector<indexType> mFreeIds;
		
	public:
		inline UniqueStorage() noexcept: mArr() {}
		
		// @todo test
		inline UniqueStorage(const UniqueStorage<T> &other) = default;

		// @todo test
		inline UniqueStorage(UniqueStorage<T> &&rv) noexcept = default;

		// @todo test
		inline UniqueStorage(std::initializer_list<T> _lst): mArr(_lst), mActive(mArr.length()) {
			mActive.assign(mActive.size(), true);
		}
		
		inline UniqueStorage<T> &operator =(const UniqueStorage<T> &other) = default;
		inline UniqueStorage<T> &operator =(UniqueStorage<T> &&rv) noexcept = default;
		
		class Iterator
		{
		protected:
			UniqueStorage<T> *mSt;
			indexType mInd;
			
			inline void nextValid() {
				while (mInd != mSt->size() && !mSt->mActive[mInd])
					++mInd;
			}
			inline void prevValid() {
				while (mInd != static_cast<indexType>(-1) && !mSt->mActive[mInd])
					--mInd;
			}
			friend class UniqueStorage<T>;
			
		public:
			inline Iterator(UniqueStorage<T> *st, indexType ind):
				mSt(st), mInd(ind) {}
			
			constexpr inline T &operator *() const {
				return (*mSt)[mInd];
			}
			constexpr inline T *operator ->() const {
				return &((*mSt)[mInd]);
			}
			
			constexpr inline Iterator &operator ++() {
				CM3D_ASSERT(mInd != mSt->size());
				++mInd; nextValid();
				return *this;
			}
			constexpr inline Iterator &operator --() {
				CM3D_ASSERT(mInd != static_cast<indexType>(-1));
				--mInd; prevValid();
				return *this;
			}
			constexpr inline Iterator operator ++(int) {
				Iterator it = *this;
				operator ++();
				return it;
			}
			constexpr inline Iterator operator --(int) {
				Iterator it = *this;
				operator --();
				return it;
			}
			
			constexpr inline bool operator ==(const Iterator &it) const {
				return this->mSt == it.mSt && this->mInd == it.mInd;
			}
			constexpr inline bool operator !=(const Iterator &it) const {
				return this->mSt != it.mSt || this->mInd != it.mInd;
			}
			
			constexpr inline bool operator >(const Iterator &it) const {
				return this->mInd > it.mInd;
			}
			constexpr inline bool operator <(const Iterator &it) const {
				return this->mInd < it.mInd;
			}
			constexpr inline bool operator >=(const Iterator &it) const {
				return this->mInd >= it.mInd;
			}
			constexpr inline bool operator <=(const Iterator &it) const {
				return this->mInd <= it.mInd;
			}
			
			constexpr inline bool operator &(const Iterator &it) const {
				return this->mSt == it.mSt;
			}
			constexpr inline bool operator ^(const Iterator &it) const {
				return this->mSt != it.mSt;
			}
			
			constexpr inline indexType operator ~() const {
				return this->mInd;
			}
			constexpr inline indexType index() const {
				return this->mInd;
			}
		};
		
		class cIterator: public Iterator
		{
		public:
			inline cIterator(UniqueStorage<T> const *st, indexType ind):
				Iterator(const_cast<UniqueStorage<T> *>(st), ind) {}
			
			constexpr inline const T &operator *() const {
				return reinterpret_cast<const Iterator *>(this)->operator *();
			}
			constexpr inline const T *operator ->() const {
				return reinterpret_cast<const Iterator *>(this)->operator ->();
			}
		};
		
		constexpr inline Iterator begin() {
			auto iter = Iterator(this, 0);
			iter.nextValid();
			return iter;
		}
		constexpr inline Iterator end() {
			return Iterator(this, size());
		}
		constexpr inline cIterator begin() const {
			auto iter = cIterator(this, 0);
			iter.nextValid();
			return iter;
		}
		constexpr inline cIterator end() const {
			return cIterator(this, size());
		}
		
		constexpr inline valueType &operator[](const indexType id)
		{
			CM3D_ASSERT(mActive[id]);
			return mArr[id];
		}

		constexpr inline const valueType &operator[](const indexType id) const
		{
			CM3D_ASSERT(mActive[id]);
			return mArr[id];
		}

		// current array size, including free space after deleted objects
		constexpr inline size_t size() const { return mArr.size(); }
		
		constexpr inline size_t length() const { return mArr.size() - mFreeIds.size(); }

		inline void clear() noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			mArr.clear();
			mActive.clear();
			mFreeIds.clear();
		}
		
	protected:
		inline indexType getFreeIndex() {
			indexType ind = mFreeIds.back();
			mFreeIds.pop_back();
			CM3D_ASSERT(!mActive[ind]);
			mActive[ind] = true;
			return ind;
		}
		
	public:
		// @todo rename to insert
		// @todo remove WET
		inline indexType push(T &&elem)
		{
			if (mFreeIds.size())
			{
				indexType ind = getFreeIndex();
				mArr[ind] = std::move(elem);
				return ind;
			}
			else
			{
				mArr.push_back(std::move(elem));
				mActive.push_back(true);
				return mArr.size() - 1;
			}
		}
		
		inline indexType push(T const &elem)
		{
			if (mFreeIds.size())
			{
				indexType ind = getFreeIndex();
				mArr[ind] = elem;
				return ind;
			}
			else
			{
				mArr.push_back(elem);
				mActive.push_back(true);
				return mArr.size() - 1;
			}
		}

	protected:
		template<typename Tp>
		inline T &pInsert(Tp &&elem, indexType index)
		{
			if (indexType prev = mArr.size(); index >= prev)
			{
				mArr.resize(index + 1);
				mActive.resize(index + 1, false);
				for (indexType i = prev; i + 1 < index; ++i) {
					mFreeIds.push_back(i);
				}
			}
			mActive[index] = true;
			return mArr[index] = std::forward<Tp>(elem);
		}

	public:
		inline T &insert(T &&elem, indexType index) {
			return pInsert(std::move(elem), index);
		}
		inline T &insert(T const &elem, indexType index) {
			return pInsert(elem, index);
		}
		
		inline UniqueStorage<T> reorder() {
			UniqueStorage<T> cp {};
			for (auto const &el: *this)
				cp.push(el);
			return cp;
		}
		
		inline void erase(const indexType id)
		{
			CM3D_ASSERT(mActive[id]);
			mArr[id].~T();
			new(&mArr[id]) T(); // avoid dead objects for stable copying
			mActive[id] = false;
			mFreeIds.push_back(id);
		}
		
	protected:
		friend bitsery::Access;
		
		template<typename TOutputAdapter, typename TContext>
		void serialize(bitsery::Serializer<TOutputAdapter, TContext> &s)
		{
			using Serialization::filter_type;
			
			uint64_t v;
			v = size(); s.value8b(v);
			v = length(); s.value8b(v);

			for (auto iter = begin(); iter != end(); ++iter) {
				s.value8b(iter.index());
				s.object(filter_type(*iter));
			}

			// s.container(mArr, 1000000, [](bitsery::Serializer<TOutputAdapter, TContext> &s, T &p) {
			// 	s.object(filter_type(p));
			// });
		}

		template<typename TInputAdapter, typename TContext>
		void serialize(bitsery::Deserializer<TInputAdapter, TContext> &s)
		{
			using Serialization::filter_type;

			uint64_t len;
			s.value8b(len);
			mArr.resize(len);
			mActive.resize(len, false);

			s.value8b(len);
			for (size_t i = 0; i != (size_t)len; ++i) {
				uint64_t index;
				s.value8b(index);
				if constexpr (sizeof(size_t) < sizeof(uint64_t)) {
					if (index > SIZE_MAX) {
						throw std::runtime_error("runned out of index space");
					}
				}
				size_t ind = static_cast<size_t>(index);
				s.object(filter_type(mArr[ind]));
				mActive[ind] = true;
			}

			indexType i;
			for (bool act: mActive) {
				if (!act)
					mFreeIds.push_back(i);
				++i;
			}

			// s.container(mArr, 1000000, [](bitsery::Deserializer<TInputAdapter, TContext> &s, T &p) {
			// 	s.object(filter_type(p));
			// });
			// mActive.resize(mArr.size(), true);
		}
	};
}

#endif
