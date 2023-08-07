#ifndef INC_GLSL
#define INC_GLSL

float genRandomNumber(int seed) {
    return float(((((seed + 257) * 123) + 7434348) * 933547) + seed + 17);
}

vec3 genRandomVec3(ivec3 seed) {
	return vec3(genRandomNumber(seed.x), genRandomNumber(seed.y), genRandomNumber(seed.z));
}

#endif