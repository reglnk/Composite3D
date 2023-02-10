/** ===============================================
 * A composite scalar type, allowing to keep almost
 * constant precision on huge range of values.
**/

#ifndef CM3D_TYPES_SCALAR_HPP
#define CM3D_TYPES_SCALAR_HPP

#include <Types/Aliases.hpp>

namespace cm3d
{
	struct Scalar
	{
		sInteger lv; // linear value (metres)
		sReal rv; // clarification (-1.0; 1.0)

	/* ============================================== */

		inline Scalar() noexcept = default;
		inline Scalar(sReal val);
		
		constexpr Scalar(sInteger lv);
		constexpr Scalar(sInteger lv, sReal rv);

		template<typename T>
		constexpr explicit operator T() const;
		constexpr explicit operator bool() const;

	/* ============================================== */

		constexpr bool operator==(Scalar const &sc) const;
		constexpr bool operator!=(Scalar const &sc) const;

		constexpr bool operator>(Scalar const &sc) const;
		constexpr bool operator>=(Scalar const &sc) const;

		constexpr bool operator<(Scalar const &sc) const;
		constexpr bool operator<=(Scalar const &sc) const;

	/* ============================================== */

		constexpr Scalar operator+(Scalar sc) const;

		constexpr Scalar operator-(Scalar sc) const;

		constexpr Scalar operator*(sReal sc) const;
		constexpr Scalar operator*(Scalar sc) const;

		constexpr Scalar operator/(sReal sc) const;
		constexpr Scalar operator/(Scalar sc) const;

	/* ============================================== */

		constexpr Scalar & operator=(Scalar sc);

		constexpr Scalar & operator+=(Scalar sc);

		constexpr Scalar & operator-=(Scalar sc);

		constexpr Scalar & operator*=(sReal sc);
		constexpr Scalar & operator*=(Scalar sc);

		constexpr Scalar & operator/=(sReal sc);
		constexpr Scalar & operator/=(Scalar sc);
	};
}

#include "Inline/Scalar.inl"

#endif // CM3D_TYPES_SCALAR_HPP
