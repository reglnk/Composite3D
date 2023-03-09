#ifndef CM3D_FIRST_PERSON_CAMERA_HPP
#define CM3D_FIRST_PERSON_CAMERA_HPP

/*==================================================================
 * A First Person Camera. World-up axis can be any 3D unit vector.
 */

#include <Types/Aliases.hpp>
#include <Types/Linear.hpp>
#include <Types/Vector3.hpp>

#include <Core/Math.hpp>

// @todo
// onReachUpperLimit, onReachLowerLimit -- angle limiting callbacks

namespace cm3d
{
	class FPCamera
	{
	public:
		class MovDirection {
			MovDirection() = delete;
			~MovDirection() = delete;
		public:
			constexpr static int Forward  = 0x1;
			constexpr static int Backward = 0x2;
			constexpr static int Left     = 0x4;
			constexpr static int Right    = 0x8;
			constexpr static int relUp    = 0x10;
			constexpr static int relDown  = 0x20;
			constexpr static int absUp    = 0x40;
			constexpr static int absDown  = 0x80;
		};
		enum ViewMode {
			Fixed = 0, // unitUpAbsolute isn't changed automatically
			Free = 1, // may change by rotation
			Pivot = 2 // pointing from pivot to wPos
		};

		float FoV; // radians
		float frustumNear;
		float frustumFar;
		sReal offsetRsp; // response to mouse offset
		sReal accFactor; // acceleration
		sReal dmpFactor; // damping
		
		Vector3 wPos;
		glm::crvec3 wVel;

		inline FPCamera() = default;
		
		inline FPCamera
		(
			Vector3 wPos = Vector3(0.0),
			glm::crvec3 wVel = glm::crvec3(0.0),
			glm::crvec3 forwardDir = glm::crvec3(0.0, 0.0, 1.0),
			glm::crvec3 upDir = glm::crvec3(0.0, 1.0, 0.0),
			float FoV = 1.57f,
			float frustumFar = 300.0f,
			sReal offsetResponse = 0.017,
			sReal accFactor = 40.0,
			sReal dmpFactor = 4.0,
			Vector3 *pivot = NULL,
			ViewMode mode = Free
		):
			wPos(wPos), wVel(wVel),
			unitForward(forwardDir), unitUp(upDir), unitUpAbsolute(upDir),
			FoV(FoV), frustumFar(frustumFar), 
			accFactor(accFactor), dmpFactor(dmpFactor), offsetRsp(offsetResponse),
			pivotPoint(pivot), vMode(mode)
		{
			// if initialized as free, then update just angle
			// otherwise look in update & update unitUpAbsolute
			verticalAngle = std::acos(glm::dot(upDir, forwardDir));
			update();
		}
		
		inline glm::mat4 viewMatrix(float aspect)
		{
			// @todo rework wPos behaviour so that the matrix precision won't be
			// awful at very far point from world center
			using namespace glm;
			vec3 w_pos(wPos.x, wPos.y, wPos.z);
			mat4 look_at = lookAt(w_pos, w_pos + vec3(unitForward), vec3(unitUp));
			return perspective(FoV, aspect, 0.1f, frustumFar) * look_at;
		}
		inline glm::dmat4 viewMatrix(sReal aspect)
		{
			using namespace glm;
			dvec3 w_pos(wPos.x, wPos.y, wPos.z);
			dmat4 look_at = lookAt(w_pos, w_pos + dvec3(unitForward), dvec3(unitUp));
			return perspective((double)FoV, (double)aspect, 0.1, (double)frustumFar) * look_at;
		}
		
		void applyVelDelta(const int direction, const sReal dTime);
		void offsetCallback(const double off_x, const double off_y);
		void scrollCallback(const double off_x, const double off_y, const int glfwMods);
		
		void processPhysics(const sReal dTime);
		void update();

		inline const sReal getVerticalAngle() const {
			return verticalAngle;
		}
		inline glm::crvec3 const &getUp() const {
			return unitUp;
		}
		inline glm::crvec3 const &getForward() const {
			return unitForward;
		}
		inline glm::crvec3 const &getLeft() const {
			return unitLeft;
		}
		inline glm::crvec3 const &getUpAbsolute() const {
			return unitUpAbsolute;
		}

		inline void setVerticalAngle(const sReal angle) {
			verticalAngle = angle;
			update();
		}
		inline void setUnitForward(glm::crvec3 const &forward) {
			verticalAngle = std::acos(glm::dot(unitUpAbsolute, forward));
			unitForward = forward;
			update();
		}

	protected:
		sReal verticalAngle; // angle between unitUpAbsolute & unitForward
		
		glm::crvec3 unitForward;
		glm::crvec3 unitUp; // current up direction used to calculate lookAt
		glm::crvec3 unitLeft; // == glm::cross(unitUp, unitForward)
		glm::crvec3 unitUpAbsolute;

		Vector3 *pivotPoint;
		ViewMode vMode;
	};
} // namespace cm3d

#endif // FIRST_PERSON_CAMERA_HPP
