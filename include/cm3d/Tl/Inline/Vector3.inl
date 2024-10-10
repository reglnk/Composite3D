#include <cstddef>

namespace cm3d
{
	inline Scalar &Vector3::operator[](unsigned i) {
		constexpr Scalar Vector3::*accessors[] = {
            &Vector3::x,
            &Vector3::y,
            &Vector3::z,
        };
        return this->*accessors[i];
	}
	inline Scalar const &Vector3::operator[](unsigned i) const {
		constexpr Scalar Vector3::*accessors[] = {
            &Vector3::x,
            &Vector3::y,
            &Vector3::z,
        };
        return this->*accessors[i];
	}

/* ================================================ */

	constexpr Vector3::Vector3(Vector3 const &v) :
		x(v.x), y(v.y), z(v.z) {}

	constexpr Vector3::Vector3(Scalar const &sc) :
		x(sc), y(sc), z(sc) {}
	
	constexpr Vector3::Vector3(Scalar const &x, Scalar const &y, Scalar const &z) :
		x(x), y(y), z(z) {}
	
	template<typename T, glm::qualifier Q>
	constexpr Vector3::Vector3(glm::vec<3, T, Q> v):
		x(v.x), y(v.y), z(v.z) {}

/* ================================================ */

	// constexpr Vector3::operator glm::crvec3() const {
	// 	return glm::crvec3(x, y, z);
	// }
}

