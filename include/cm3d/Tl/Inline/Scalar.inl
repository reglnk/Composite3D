#include <cmath>

namespace cm3d
{
	template<typename T>
	constexpr Scalar::operator T() const {
		return static_cast<T>(lv) + static_cast<T>(rv);
	}

	constexpr Scalar::operator bool() const {
		return lv && rv;
	}
	
/* ============================================== */

	constexpr bool Scalar::operator ==(Scalar const& sc) const {
		return lv == sc.lv && rv == sc.rv;
	}
	constexpr bool Scalar::operator !=(Scalar const& sc) const {
		return lv != sc.lv || rv != sc.rv;
	}

	constexpr bool Scalar::operator >(Scalar const& sc) const {
		return lv > sc.lv ||
			(lv == sc.lv && rv > sc.rv);
	}
	constexpr bool Scalar::operator >=(Scalar const& sc) const {
		return lv > sc.lv ||
			(lv == sc.lv && rv >= sc.rv);
	}

	constexpr bool Scalar::operator <(Scalar const& sc) const {
		return lv < sc.lv ||
			(lv == sc.lv && rv < sc.rv);
	}
	constexpr bool Scalar::operator <=(Scalar const& sc) const {
		return lv < sc.lv ||
			(lv == sc.lv && rv <= sc.rv);
	}

/* ============================================== */

	constexpr Scalar Scalar::operator +(Scalar const &sc) const {
		Scalar nsc = {lv + sc.lv, rv + sc.rv};
		if (std::abs(nsc.rv) >= static_cast<sReal>(1.0)) {
			int sb = static_cast<int>(std::signbit(nsc.rv));
			sb <<= 1; sb -= 1;
			nsc.lv -= sb;
			nsc.rv += static_cast<sReal>(sb);
		}
		return nsc;
	}
	constexpr Scalar Scalar::operator -(Scalar const &sc) const {
		Scalar nsc = {lv - sc.lv, rv - sc.rv};
		if (std::abs(nsc.rv) >= static_cast<sReal>(1.0)) {
			int sb = static_cast<int>(std::signbit(nsc.rv));
			sb <<= 1; sb -= 1;
			nsc.lv -= sb;
			nsc.rv += static_cast<sReal>(sb);
		}
		return nsc;
	}
	constexpr Scalar Scalar::operator *(sReal sc) const {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val *= sc;

		sInteger int_part = static_cast<sInteger>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		Scalar nsc = {int_part, diff};
		return nsc;
	}
	constexpr Scalar Scalar::operator *(Scalar const &sc) const {
		return operator *(static_cast<sReal>(sc));
	}
	constexpr Scalar Scalar::operator /(sReal sc) const {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val *= sc;

		sInteger int_part = static_cast<sInteger>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		Scalar nsc = {int_part, diff};
		return nsc;
	}
	constexpr Scalar Scalar::operator /(Scalar const &sc) const {
		return operator /(static_cast<sReal>(sc));
	}

/* ============================================== */

	constexpr Scalar &Scalar::operator +=(Scalar const &sc) {
		lv += sc.lv;
		rv += sc.rv;
		if (std::fabs(rv) >= static_cast<sReal>(1.0)) {
			int sb = static_cast<int>(std::signbit(rv));
			sb <<= 1; sb -= 1;
			lv -= sb;
			rv += static_cast<sReal>(sb);
		}
		return *this;
	}

	constexpr Scalar &Scalar::operator -=(Scalar const &sc) {
		lv -= sc.lv;
		rv -= sc.rv;
		if (std::fabs(rv) >= static_cast<sReal>(1.0)) {
			int sb = static_cast<int>(std::signbit(rv));
			sb <<= 1; sb -= 1;
			lv -= sb;
			rv += static_cast<sReal>(sb);
		}
		return *this;
	}

	constexpr Scalar &Scalar::operator *=(sReal sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val *= sc;

		sInteger int_part = static_cast<sInteger>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		lv = int_part;
		rv = diff;
		return *this;
	}
	constexpr Scalar &Scalar::operator *=(Scalar const &sc) {
		return operator *=(static_cast<sReal>(sc));
	}

	constexpr Scalar &Scalar::operator /=(sReal sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val /= sc;

		sInteger int_part = static_cast<sInteger>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		lv = int_part;
		rv = diff;
		return *this;
	}
	constexpr Scalar &Scalar::operator /=(Scalar const &sc) {
		return operator /=(static_cast<sReal>(sc));
	}
}
