#ifndef CM3D_VECTOR3_HPP
#define CM3D_VECTOR3_HPP

#include <cm3d/Tl/Scalar.hpp>
#include <cm3d/Tl/LinearAliases.hpp>

// @todo think on some possibility to use common glm::vec3 or smth like that for small scenes

namespace cm3d
{
	struct Vector3
	{
		Scalar x;
		Scalar y;
		Scalar z;

	/* ================================================ */
		
		constexpr inline unsigned size() { return 3; }
		
		inline Scalar &operator[](unsigned i);
		inline Scalar const &operator[](unsigned i) const;

	/* ================================================ */

		constexpr inline Vector3(): x(), y(), z() {}
		constexpr Vector3(Vector3 const &v);

		constexpr explicit Vector3(Scalar const &sc);
		constexpr Vector3(Scalar const &x, Scalar const &y, Scalar const &z);
		
		template<typename T, glm::qualifier Q>
		constexpr Vector3(glm::vec<3, T, Q> v);

	/* ================================================ */

		constexpr explicit operator bool() const {
			return x && y && z;
		}
	
	/* ================================================ */

		constexpr bool operator ==(Vector3 const &v) const {
			return x == v.x && y == v.y && z == v.z;
		}
		constexpr bool operator !=(Vector3 const &v) const {
			return x != v.x || y != v.y || z != v.z;
		}

		sReal length() const {
			glm::crvec3 crv(x, y, z);
			return glm::length(crv);
		}

		bool operator >(Vector3 const &v) const;
		bool operator >=(Vector3 const &v) const;

		bool operator <(Vector3 const &v) const;
		bool operator <=(Vector3 const &v) const;

	/* ================================================ */

		constexpr Vector3 &operator =(Vector3 const &v) {
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		Vector3 &operator +=(Vector3 const &v);

		Vector3 &operator -=(Vector3 const &v);

		Vector3 &operator *=(sReal const &v);

		Vector3 &operator /=(sReal const &v);

		Vector3 &operator *=(Scalar const &sc) {
			return operator *=(static_cast<sReal>(sc));
		}
		Vector3 &operator /=(Scalar const &sc) {
			return operator /=(static_cast<sReal>(sc));
		}

	/* ================================================ */

		Vector3 operator +(Vector3 const &v) const;

		Vector3 operator -(Vector3 const &v) const;

		Vector3 operator *(sReal const &v) const;

		Vector3 operator /(sReal const &v) const;

		Vector3 operator *(Scalar const &sc) const {
			return operator *(static_cast<sReal>(sc));
		}
		Vector3 operator /(Scalar const &sc) const {
			return operator /(static_cast<sReal>(sc));
		}
	};
}

#include "Inline/Vector3.inl"

#endif // CM3D_VECTOR3_HPP
