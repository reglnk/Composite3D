#ifndef CM3D_FIRST_PERSON_CAMERA_HPP
#define CM3D_FIRST_PERSON_CAMERA_HPP

/*==================================================================
 * First Person Camera. World-up axis can be any 3D unit vector.
 */

#include <Types/Aliases.hpp>
#include <Types/Linear.hpp>
#include <Types/Vector3.hpp>

#include <Core/Math.hpp>

// @TODO: customizable functions
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
		float renderDist;
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
			float renderDist = 300.0f,
			sReal offsetResponse = 0.017,
			sReal accFactor = 40.0,
			sReal dmpFactor = 4.0,
			Vector3 *pivot = NULL,
			ViewMode mode = Free
		);
		
		// @todo explicit float & double variants?
		glm::mat<4, 4, sReal> viewMatrix(sReal aspect);
		
		void applyVelDelta(const int direction, const sReal dTime);
		void offsetCallback(const double off_x, const double off_y);
		void scrollCallback(const double off_x, const double off_y, const int glfwMods);
		
		void processPhysics(const sReal dTime);
		void updateDirections();

		constexpr const sReal getVerticalAngle() const {
			return verticalAngle;
		}
		constexpr glm::crvec3 const &getUp() const {
			return unitUp;
		}
		constexpr glm::crvec3 const &getForward() const {
			return unitForward;
		}
		constexpr glm::crvec3 const &getLeft() const {
			return unitLeft;
		}
		constexpr glm::crvec3 const &getUpAbsolute() const {
			return unitUpAbsolute;
		}

		inline void setVerticalAngle(const sReal angle) {
			verticalAngle = angle;
			updateDirections();
		}
		inline void setUnitForward(glm::crvec3 const &forward) {
			verticalAngle = std::acos(glm::dot(unitUpAbsolute, forward));
			unitForward = forward;
			updateDirections();
		}

	private:
		sReal verticalAngle; // angle between unitUpAbsolute & unitForward
		
		glm::crvec3 unitForward;
		glm::crvec3 unitUp; // current up direction used to calculate lookAt
		glm::crvec3 unitLeft; // cache (= glm::cross(unitUp, unitForward))
		glm::crvec3 unitUpAbsolute;

		Vector3 *pivotPoint;
		ViewMode vMode;
	};
} // namespace cm3d

#endif // FIRST_PERSON_CAMERA_HPP
