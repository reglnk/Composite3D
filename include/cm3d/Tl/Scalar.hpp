/** ===============================================
 * A composite scalar type, allowing to keep almost
 * constant precision on huge range of values.
**/

#ifndef CM3D_TYPES_SCALAR_HPP
#define CM3D_TYPES_SCALAR_HPP

#include <cm3d/Tl/Aliases.hpp>

namespace cm3d
{
	struct Scalar
	{
		sInteger lv; // linear value (metres)
		sReal rv; // clarification (-1.0; 1.0)

	/* ============================================== */

		constexpr inline Scalar(): lv(), rv() {}

		constexpr inline Scalar(sReal val)
		: lv(static_cast<sInteger>(val)), rv(val - lv)
		{}
		
		constexpr Scalar(sInteger lv): lv(lv), rv(0.0) {}
		constexpr Scalar(sInteger lv, sReal rv): lv(lv), rv(rv) {}

		template<typename T>
		constexpr explicit operator T() const;
		constexpr explicit operator bool() const;

	/* ============================================== */

		constexpr bool operator ==(Scalar const &sc) const;
		constexpr bool operator !=(Scalar const &sc) const;

		constexpr bool operator >(Scalar const &sc) const;
		constexpr bool operator >=(Scalar const &sc) const;

		constexpr bool operator <(Scalar const &sc) const;
		constexpr bool operator <=(Scalar const &sc) const;

	/* ============================================== */

		constexpr Scalar operator +(Scalar const &sc) const;

		constexpr Scalar operator -(Scalar const &sc) const;

		constexpr Scalar operator *(sReal sc) const;
		constexpr Scalar operator *(Scalar const &sc) const;

		constexpr Scalar operator /(sReal sc) const;
		constexpr Scalar operator /(Scalar const &sc) const;

	/* ============================================== */

		constexpr Scalar &operator =(Scalar const &sc) {
			lv = sc.lv;
			rv = sc.rv;
			return *this;
		}

		constexpr Scalar &operator +=(Scalar const &sc);

		constexpr Scalar &operator -=(Scalar const &sc);

		constexpr Scalar &operator *=(sReal sc);
		constexpr Scalar &operator *=(Scalar const &sc);

		constexpr Scalar &operator /=(sReal sc);
		constexpr Scalar &operator /=(Scalar const &sc);
	};
}

#include "Inline/Scalar.inl"

#endif // CM3D_TYPES_SCALAR_HPP
