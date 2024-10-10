#ifndef CM3D_MATH_HPP
#define CM3D_MATH_HPP

#include <cm3d/Tl/Vector3.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace cm3d::Math
{
	template<typename T>
	constexpr T generate_G();

	template<typename T>
	constexpr T generate_C();

	template<typename T, glm::qualifier Q>
	T cosvec3(glm::vec<3, T, Q> const& k, glm::vec<3, T, Q> const& v);

	// return vector: x- cos, y- sin of angle
	template<typename T, glm::qualifier Q>
	glm::vec<2, T, Q> anglev3_fixedAxis
	(
		const glm::vec<3, T, Q> &b, // Before rotation
		const glm::vec<3, T, Q> &a, // After
		const glm::vec<3, T, Q> &axis // axis of rotation (counter-clockwise, normalized)
	);

	template<typename T, glm::qualifier Q>
	glm::vec<3, T, Q> rotate
	(
		glm::vec<3, T, Q> const& point,
		const T a_cos,
		const T a_sin,
		glm::vec<3, T, Q> const& r_axis // normalized, counter-clockwise
	);
	
	template<typename T, glm::qualifier Q>
	glm::mat<4, 4, T, Q> decomposeRotation
	(
		const glm::vec<3, T, Q>& k, const glm::vec<3, T, Q>& v, // Before rotation
		const glm::vec<3, T, Q>& ck, const glm::vec<3, T, Q>& cv // After
	);

	template<typename T, glm::qualifier Q>
	glm::vec<3, T, Q> gravitacc
	(
		glm::vec<3, T, Q> const& body,
		glm::vec<3, T, Q> const& dest,
		const T mass
	);
} // namespace cm3d::Math

#include "Inline/Math.inl"

#endif
