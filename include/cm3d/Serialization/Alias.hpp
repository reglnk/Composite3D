#ifndef CM3D_SERIALIZATION_ALIAS_HPP
#define CM3D_SERIALIZATION_ALIAS_HPP

// Types converter
namespace cm3d::Serialization
{
	template<typename T>
	struct Alias
	{
		using type = T;
	};
	
	template<typename T>
	constexpr inline typename Alias<T>::type &filter_type(T &obj) {
		return *reinterpret_cast<typename Alias<T>::type *>(&obj);
	}
}

#endif
