#ifndef CM3D_SERIALIZATION_TYPES_SMART_PTR_HPP
#define CM3D_SERIALIZATION_TYPES_SMART_PTR_HPP

#include <bitsery/ext/std_smart_ptr.h>
#include <cm3d/Serialization/Alias.hpp>
#include <memory>

namespace cm3d::Serialization
{
	template<typename T>
	class shared_ptr: public std::shared_ptr<T>
	{
	public:
		template<typename S>
		void serialize(S &s) {
			auto ptr = reinterpret_cast<std::shared_ptr<T> *>(this);
			s.ext(*ptr, bitsery::ext::StdSmartPtr{});
		}
		
		shared_ptr() = default;
		shared_ptr(std::shared_ptr<T> const &oth) {
			auto ptr = reinterpret_cast<std::shared_ptr<T> *>(this);
			*ptr = oth;
		}
		shared_ptr(std::shared_ptr<T> &&rv) {
			auto ptr = reinterpret_cast<std::shared_ptr<T> *>(this);
			*ptr = std::move(rv);
		}
	};
	
	// this entire header and Alias.hpp are made as a workaround to serialize
	// std::shared_ptr with bitsery::Serializer::object function
	template<typename T>
	struct Alias<std::shared_ptr<T>>
	{
		using type = shared_ptr<T>;
	};
}

#endif
