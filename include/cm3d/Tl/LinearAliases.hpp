#ifndef CM3D_CRVEC3_HPP
#define CM3D_CRVEC3_HPP

#include <cm3d/Tl/Aliases.hpp>
#include <glm/glm.hpp>

namespace glm
{
	typedef vec<2, cm3d::sReal> crvec2;
	typedef vec<3, cm3d::sReal> crvec3;
	typedef vec<4, cm3d::sReal> crvec4;

	typedef mat<3, 3, cm3d::sReal> crmat3;
	typedef mat<4, 4, cm3d::sReal> crmat4;
}

#endif