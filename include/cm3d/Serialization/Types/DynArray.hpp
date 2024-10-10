#ifndef CM3D_SERIALIZATION_TYPES_DYNARRAY_HPP
#define CM3D_SERIALIZATION_TYPES_DYNARRAY_HPP

#include <bitsery/details/serialization_common.h>
#include <cm3d/Tl/DynArray.hpp>

namespace bitsery::traits
{
	template<typename T>
	struct ContainerTraits<cm3d::DynArray<T>>
	{
		using TValue = typename cm3d::DynArray<T>::valueType;
		static constexpr bool isResizable = true;
		static constexpr bool isContiguous = true;
		static size_t size(const cm3d::DynArray<T>& container) {
			return container.length();
		}
		static void resize(cm3d::DynArray<T>& container, size_t size) {
			container.resize(size);
		}
	};

	// mostly copied from <bitsery/traits/core/std_defaults.h>
	template<typename T>
	struct BufferAdapterTraits<cm3d::DynArray<T>>
	{
		using TIterator = typename T::iterator;
		using TConstIterator = typename T::const_iterator;
		using TValue = typename ContainerTraits<T>::TValue;

		static void increaseBufferSize (
			T& container,
			size_t /*currSize*/,
			size_t minSize
		) {
			auto newSize = static_cast<size_t>(static_cast<double>(container.length()) * 1.5) + 128;
			newSize -= newSize % 64;
			container.resize (
				(std::max)(newSize > minSize ? newSize : minSize, container.capacity())
			);
		}
	};
}

#endif
