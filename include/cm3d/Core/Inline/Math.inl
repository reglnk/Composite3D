#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace cm3d
{
	namespace Math
	{
		template<typename T>
		constexpr T generate_G() {
			return static_cast<T>(6.674301515e-11);
		}

		template<typename T>
		constexpr T generate_C() {
			return static_cast<T>(299792458.0);
		}

		template<typename T, glm::qualifier Q>
		T cosvec3(glm::vec<3, T, Q> const& v, glm::vec<3, T, Q> const& av) {
			return glm::dot(v, av) / (sqrt(glm::dot(v, v) * glm::dot(av, av)));
		}

		// return vector: x- cos, y- sin of angle
		template<typename T, glm::qualifier Q>
		glm::vec<2, T, Q> anglev3_fixedAxis
		(
			const glm::vec<3, T, Q> &b, // Before rotation
			const glm::vec<3, T, Q> &a, // After
			const glm::vec<3, T, Q> &axis // axis of rotation (counter-clockwise)
		)
		{
			glm::vec<2, T, Q> res;
			res.x = cosvec3(b, a);

			glm::vec<3, T, Q> cr = glm::cross(b, a);
			res.y = glm::length(cr); // get sin for minimal angle
			if (res.y < glm::epsilon<T>())
				return res;

			cr *= static_cast<T>(1.0) / res.y; // normalize
			res.y *= glm::dot(cr, axis); // multiply sin by -1 if axes aren't same
			return res;
		}

		template<typename T, glm::qualifier Q>
		glm::vec<3, T, Q> rotate
		(
			glm::vec<3, T, Q> const& point,
			const T a_cos,
			const T a_sin,
			glm::vec<3, T, Q> const& r_axis // normalized, counter-clockwise
		) {
			const glm::vec<3, T, Q> sub = point - r_axis * dot(r_axis, point);
			return point + glm::cross(r_axis, sub) * a_sin
				+ sub * (a_cos - static_cast<T>(1.0));
		}

		template<typename T, glm::qualifier Q>
		glm::mat<4, 4, T, Q> decomposeRotation
		(
			const glm::vec<3, T, Q>& k, const glm::vec<3, T, Q>& v, // Before rotation
			const glm::vec<3, T, Q>& ck, const glm::vec<3, T, Q>& cv // After
		)
		{
		/* =======================================================
		 * There are two rotations:
		 *   if the 1st is applied, then k' = ck and v' = cvm.
		 *   the second acts so that k' = k and cvm' = cv.
		 * These rotations are then combined to derive matrix.
		 *
		 * @TODO: optimize
		** ======================== */

			T angle1, angle2;
			glm::vec<3, T, Q> axis1, axis2;

			glm::vec<3, T, Q> cvm;

			// Points for 2nd rotation decomposition
			glm::vec<3, T, Q> cvi, cvl;

			double a1_cos = cosvec3(k, ck);
			double a1_sin = std::sqrt(static_cast<T>(1.0) - a1_cos * a1_cos);

			axis1 = glm::normalize(glm::cross(k, ck));
			cvm = rotate(v, a1_cos, a1_sin, axis1);
			
			axis2 = glm::normalize(ck);
			
			// Offset points to make them perpend to ck
			cvi = cvm - axis2 * (glm::dot(cvm, axis2));
			cvl = cv - axis2 * (glm::dot(cv, axis2));
			
			double a2 = std::acos(cosvec3(cvi, cvl));
			glm::vec<3, T, Q> axis3 = glm::normalize(glm::cross(cvi, cvl));
			
			// Combine transforms
			glm::mat<4, 4, T, Q> tf(1.0);
			tf = glm::rotate(tf, a2, axis3);
			tf = glm::rotate(tf, std::acos(a1_cos), axis1);
			
			return tf;
		}

		template<typename T, glm::qualifier Q>
		glm::vec<3, T, Q> gravitacc
		(
			glm::vec<3, T, Q> const& body,
			glm::vec<3, T, Q> const& dest,
			const T mass
		) {
			const glm::vec<3, T, Q> diff = dest - body;
			T dist3 = dot(diff, diff);
			dist3 *= std::sqrt(dist3);
			return diff * (mass * 6.674301515e-11 / dist3);
		}
	} // namespace Math
} // namespace cm3d
