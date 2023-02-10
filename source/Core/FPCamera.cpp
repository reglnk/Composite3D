#include <Core/FPCamera.hpp>
#include <Core/Math.hpp>
#include <Types/Linear.hpp>

#include <GLFW/glfw3.h>

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace cm3d
{
	FPCamera::FPCamera
	(
		Vector3 wPos,
		glm::crvec3 wVel,
		glm::crvec3 forwardDir,
		glm::crvec3 upDir,
		float FoV,
		float renderDist,
		sReal offsetResponse,
		sReal accFactor,
		sReal dmpFactor,
		Vector3 *pivot,
		ViewMode mode
	):
		wPos(wPos), wVel(wVel),
		unitForward(forwardDir), unitUp(upDir), unitUpAbsolute(upDir),
		FoV(FoV), renderDist(renderDist), 
		accFactor(accFactor), dmpFactor(dmpFactor), offsetRsp(offsetResponse),
		pivotPoint(pivot), vMode(mode)
	{
		// if initialized as free, then update just angle
		// otherwise look in updateDirections & update unitUpAbsolute
		verticalAngle = std::acos(glm::dot(upDir, forwardDir));
		updateDirections();
	}

	glm::mat<4, 4, sReal> FPCamera::viewMatrix(sReal aspect)
	{
		using namespace glm;
		crvec3 w_pos(wPos.x, wPos.y, wPos.z);
		mat<4, 4, sReal> look_at = lookAt(w_pos, w_pos + unitForward, unitUp);
		return perspective((sReal)FoV, (sReal)aspect, (sReal)0.1, (sReal)renderDist) * look_at;
	}

	void FPCamera::applyVelDelta(const int direction, const sReal dTime)
	{
		using namespace glm;
		using md = MovDirection;

		vec3 mov(0.f);
		
		if (direction & (md::Forward | md::Backward))
		{
			mov = normalize(cross(unitLeft, unitUpAbsolute));
			if (direction & md::Backward)
				mov = -mov;
		}
		if (direction & md::Left)
			mov += unitLeft;
		if (direction & md::Right)
			mov -= unitLeft;
		if (direction & md::absUp)
			mov += unitUpAbsolute;
		else if (direction & md::absDown)
			mov -= unitUpAbsolute;
		else if (direction & md::relUp)
			mov += unitUp;
		else if (direction & md::relDown)
			mov -= unitUp;
		
		if (mov != glm::vec3(0.f))
			wVel += crvec3(normalize(mov)) * (dTime * accFactor);
	}

	void FPCamera::offsetCallback(const double off_x, const double off_y)
	{
		using namespace glm;
		
		verticalAngle += static_cast<sReal>(off_y * offsetRsp);
		if (verticalAngle > 3.14)
			verticalAngle = 3.14;
		else if (verticalAngle < 0.01)
			verticalAngle = 0.01;
		
		if (off_x != static_cast<sReal>(0.0))
		{
			crmat3 ryaw(glm::rotate(
				crmat4(1.0),
				static_cast<sReal>(off_x * offsetRsp),
				unitUpAbsolute
			));
			unitLeft = vec3(ryaw * unitLeft);
			unitForward = vec3(ryaw * unitForward);
		}
		updateDirections();
	}

	void FPCamera::scrollCallback(const double off_x, const double off_y, const int glfwMods)
	{
		if (glfwMods & GLFW_MOD_ALT)
		{
			FoV -= off_y * (sReal)0.03;
			if (FoV < (sReal)0.07)
				FoV = (sReal)0.07;
			else if (FoV > (sReal)3.14)
				FoV = (sReal)3.14;
		}
		else {
			renderDist += (sReal)25.0 * off_y;
			if (renderDist < (sReal)25.0)
				renderDist = (sReal)25.0;
		}
	}

	void FPCamera::processPhysics(const sReal dTime)
	{
		using namespace glm;
		if (wVel != crvec3(0.0))
		{
			crvec3 vel = wVel;
			wVel -= normalize(wVel) * (dmpFactor * dTime);
			if (dot(vel, wVel) < (sReal)0.3)
				wVel = crvec3(0.0);
			
			wPos += wVel * dTime;
		}
		updateDirections();
	}

	void FPCamera::updateDirections()
	{
		// @todo
		// 1. update absolute-up vector by pivot point (if necessary)
		// 2. limit vectors by max angles
		// 3. synchronize forward-vector by angle
		if (vMode == Pivot)
		{
			// glm::crvec3 pivot(pivotPoint->x, pivotPoint->y, pivotPoint->z);
			Vector3 diff = wPos - *pivotPoint;
			unitUpAbsolute = glm::normalize(glm::crvec3(diff.x, diff.y, diff.z));
		}
		// glm::faceforward(...);
		
		unitLeft = glm::normalize(glm::cross(unitUpAbsolute, unitForward));
		unitForward = Math::rotate(
			unitUpAbsolute,
			std::cos(verticalAngle),
			std::sin(verticalAngle),
			unitLeft
		);
	}
} // namespace cm3d