#include <cstddef>

namespace cm3d
{
	inline Scalar & Vector3::operator[](unsigned i) {
		constexpr Scalar Vector3::*accessors[] = {
            &Vector3::x,
            &Vector3::y,
            &Vector3::z,
        };
        return this->*accessors[i];
	}
	inline Scalar const& Vector3::operator[](unsigned i) const {
		constexpr Scalar Vector3::*accessors[] = {
            &Vector3::x,
            &Vector3::y,
            &Vector3::z,
        };
        return this->*accessors[i];
	}

/* ================================================ */

	constexpr Vector3::Vector3(Vector3 const& v) :
		x(v.x), y(v.y), z(v.z) {}

	constexpr Vector3::Vector3(Scalar sc) :
		x(sc), y(sc), z(sc) {}
	
	constexpr Vector3::Vector3(Scalar x, Scalar y, Scalar z) :
		x(x), y(y), z(z) {}
	
	template<typename T, glm::qualifier Q>
	constexpr Vector3::Vector3(glm::vec<3, T, Q> v):
		x(v.x), y(v.y), z(v.z) {}

/* ================================================ */

	// constexpr Vector3::operator glm::crvec3() const {
	// 	return glm::crvec3(x, y, z);
	// }

	constexpr Vector3:: operator bool() const {
		return x && y && z;
	}

/* ================================================ */

	constexpr bool Vector3::operator==(Vector3 const& v) const {
		return x == v.x && y == v.y && z == v.z;
	}
	constexpr bool Vector3::operator!=(Vector3 const& v) const {
		return x != v.x || y != v.y || z != v.z;
	}

/* ================================================ */

	constexpr Vector3 & Vector3::operator=(Vector3 const& v) {
		*this = v;
		return *(this);
	}

	inline Vector3 & Vector3::operator*=(Scalar const& sc) {
		return Vector3::operator*=(static_cast<sReal>(sc));
	}
	inline Vector3 & Vector3::operator*=(Vector3 const& v) {
		return Vector3::operator*=(glm::crvec3(v.x, v.y, v.z));
	}
	inline Vector3 & Vector3::operator/=(Scalar const& sc) {
		return Vector3::operator/=(static_cast<sReal>(sc));
	}
	inline Vector3 & Vector3::operator/=(Vector3 const& v) {
		return Vector3::operator/=(glm::crvec3(v.x, v.y, v.z));
	}

/* ================================================ */

	inline Vector3 Vector3::operator*(Scalar const& sc) const {
		return operator*(static_cast<sReal>(sc));
	}
	inline Vector3 Vector3::operator*(Vector3 const& v) const {
		return operator*(glm::crvec3(v.x, v.y, v.z));
	}
	inline Vector3 Vector3::operator/(Scalar const& sc) const {
		return operator/(static_cast<sReal>(sc));
	}
	inline Vector3 Vector3::operator/(Vector3 const& v) const {
		return operator/(glm::crvec3(v.x, v.y, v.z));
	}
}

