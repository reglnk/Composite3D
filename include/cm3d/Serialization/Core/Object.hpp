#ifndef CM3D_SERIALIZATION_OBJECT_HPP
#define CM3D_SERIALIZATION_OBJECT_HPP

#include <cm3d/Tl/Aliases.hpp>
#include <cm3d/Core/Object.hpp>

#include <cm3d/Modules/RenderSurface.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/details/serialization_common.h>
#include <bitsery/adapter/buffer.h>

#include <stdexcept>


template<typename TOutputAdapter, typename TContext>
void serialize (
	bitsery::Serializer<TOutputAdapter, TContext>& s,
	cm3d::Modules::RenderSurface const &rs
) {
	// @todo
}

template<typename TOutputAdapter, typename TContext>
void serialize (
	bitsery::Serializer<TOutputAdapter, TContext>& s,
	cm3d::Object::pModule const& m
) {
	switch (m->getId())
	{
		case cm3d::Modules::RenderSurface::Id:
			s.object(*dynamic_cast<cm3d::Modules::RenderSurface *>(m.get()));
			return;
		default:
			throw std::runtime_error("invalid module ID");
	}
}

#endif
