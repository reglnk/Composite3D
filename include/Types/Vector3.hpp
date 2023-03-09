#ifndef CM3D_VECTOR3_HPP
#define CM3D_VECTOR3_HPP

#include <Types/Scalar.hpp>
#include <Types/LinearAliases.hpp>

namespace cm3d
{
	struct Vector3
	{
		Scalar x;
		Scalar y;
		Scalar z;

	/* ================================================ */
		
		constexpr unsigned size() {return 3;}
		
		inline Scalar & operator[](unsigned i);
		inline Scalar const& operator[](unsigned i) const;

	/* ================================================ */

		inline Vector3() = default;
		constexpr Vector3(Vector3 const& v);

		constexpr explicit Vector3(Scalar sc);
		constexpr Vector3(Scalar x, Scalar y, Scalar z);
		
		template<typename T, glm::qualifier Q>
		constexpr Vector3(glm::vec<3, T, Q> v);

	/* ================================================ */

		constexpr explicit operator bool() const;
	
	/* ================================================ */

		constexpr bool operator==(Vector3 const& v) const;
		constexpr bool operator!=(Vector3 const& v) const;

		bool operator>(Vector3 const& v) const;
		bool operator>=(Vector3 const& v) const;

		bool operator<(Vector3 const& v) const;
		bool operator<=(Vector3 const& v) const;

	/* ================================================ */

		constexpr Vector3 & operator=(Vector3 const& v);

		Vector3 & operator+=(Vector3 const& v);

		Vector3 & operator-=(Vector3 const& v);

		Vector3 & operator*=(sReal const& v);
		Vector3 & operator*=(glm::crvec3 const& v);

		inline Vector3 & operator*=(Scalar const& v);
		inline Vector3 & operator*=(Vector3 const& v);

		Vector3 & operator/=(sReal const& v);
		Vector3 & operator/=(glm::crvec3 const& v);

		inline Vector3 & operator/=(Scalar const& v);
		inline Vector3 & operator/=(Vector3 const& v);

	/* ================================================ */

		Vector3 operator+(Vector3 const& v) const;

		Vector3 operator-(Vector3 const& v) const;

		Vector3 operator*(sReal const& v) const;
		Vector3 operator*(glm::crvec3 const& v) const;

		inline Vector3 operator*(Scalar const& v) const;
		inline Vector3 operator*(Vector3 const& v) const;

		Vector3 operator/(sReal const& v) const;
		Vector3 operator/(glm::crvec3 const& v) const;
		
		inline Vector3 operator/(Scalar const& v) const;
		inline Vector3 operator/(Vector3 const& v) const;
	};
}

#include "Inline/Vector3.inl"

#endif // CM3D_VECTOR3_HPP
