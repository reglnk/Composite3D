// 06 Nov, 2022
// @todo benchmark and move some cache-friendly functions back to header (if there're ones)

#include <Types/Vector3.hpp>

namespace cm3d
{
	bool Vector3::operator>(Vector3 const& v) const {
		glm::crvec3 rr(x, y, z);
		glm::crvec3 vr(v.x, v.y, v.z);

		return glm::dot(rr, rr) >= glm::dot(vr, vr);
	}
	bool Vector3::operator>=(Vector3 const& v) const {
		glm::crvec3 rr(x, y, z);
		glm::crvec3 vr(v.x, v.y, v.z);

		return glm::dot(rr, rr) >= glm::dot(vr, vr);
	}

	bool Vector3::operator<(Vector3 const& v) const {
		glm::crvec3 rr(x, y, z);
		glm::crvec3 vr(v.x, v.y, v.z);

		return glm::dot(rr, rr) < glm::dot(vr, vr);
	}
	bool Vector3::operator<=(Vector3 const& v) const {
		glm::crvec3 rr(x, y, z);
		glm::crvec3 vr(v.x, v.y, v.z);

		return glm::dot(rr, rr) <= glm::dot(vr, vr);
	}

/* ================================================ */

	Vector3 & Vector3::operator+=(Vector3 const& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3 & Vector3::operator-=(Vector3 const& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3 & Vector3::operator*=(sReal const& v) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	Vector3 & Vector3::operator*=(glm::crvec3 const& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3 & Vector3::operator/=(sReal const& v) {
		sReal inv = static_cast<sReal>(1.0) / v;
		x *= inv;
		y *= inv;
		z *= inv;
		return *this;
	}

	Vector3 & Vector3::operator/=(glm::crvec3 const& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

/* ================================================ */

	Vector3 Vector3::operator+(Vector3 const& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 Vector3::operator-(Vector3 const& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 Vector3::operator*(sReal const& v) const {
		sReal mul = v;
		return Vector3(x * mul, y * mul, z * mul);
	}

	Vector3 Vector3::operator*(glm::crvec3 const& v) const {
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	Vector3 Vector3::operator/(sReal const& v) const {
		sReal inv = static_cast<sReal>(1.0) / v;
		return Vector3(x * inv, y * inv, z * inv);
	}

	Vector3 Vector3::operator/(glm::crvec3 const& v) const {
		return Vector3(x / v.x, y / v.y, z / v.z);
	}
} // namespace cm3d
