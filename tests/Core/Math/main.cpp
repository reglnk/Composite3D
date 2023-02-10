// The test aimed to detect appropriate range of vector values to not produce Nan or inf.
// Possible range of values is set only by genRandFloat() and genRandDouble().

#include <cstdint>
#define RAND_MAX UINT32_MAX

#include <Core/Math.hpp>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <random>

#include <glm/glm.hpp>

using namespace cm3d;

constexpr uint64_t F64_SIG = 0x8000000000000000;
constexpr uint64_t F64_EXP = 0x7FF0000000000000;
constexpr uint64_t F64_MAN = 0x000FFFFFFFFFFFFF;
constexpr uint64_t F32_SIG = 0x80000000;
constexpr uint64_t F32_EXP = 0x7F800000;
constexpr uint64_t F32_MAN = 0x007FFFFF;

std::mt19937 randGen;

float genRandFloat()
{
	uint32_t u = randGen();
	
	// avoid nan and inf
	//if ((u & F32_EXP) == F32_EXP)
	//	u ^= 0x00800000;

	u &= 0x9FFFFFFF; // exponent is in range [0, 63]
	u += 0x40000000; // add 128
	u -= 0x10000000; // substract 32
	
	// u |= 0x40000000; // keep it not less than 128
	
	return *(float*)&u;
}
double genRandDouble()
{
	uint64_t u = randGen();
	u <<= 32;
	u |= randGen();

	// avoid nan and inf
	//if ((u & F64_EXP) == F64_EXP)
	//	u ^= 0x0010000000000000;

	u &= 0x9FFFFFFFFFFFFFFF; // exponent is in range [0, 511]
	u += 0x4000000000000000; // add 1024
	u -= 0x1000000000000000; // substract 256
	
	// u |= 0x4000000000000000; // keep it not less than 1024
	
	return *(double*)&u;
}

glm::vec3 genRandVec3() {
	return glm::vec3(genRandFloat(), genRandFloat(), genRandFloat());
}
glm::dvec3 genRandDvec3() {
	return glm::dvec3(genRandDouble(), genRandDouble(), genRandDouble());
}

int test_cosvec3(int c)
{
	for (; c; c--)
	{
		auto v1 = genRandVec3();
		auto v2 = genRandVec3();
		auto num = Math::cosvec3(v1, v2);
		
		// The error will appear if the vectors' lengths quotient is too high/low
		if (std::isnan(num) || std::isinf(num))
		{
			std::cout << "cosvec3 returned " << num << ":\n\t("
				<< v1.x << ", " << v1.y << ", " << v1.z << ")\n\t("
				<< v2.x << ", " << v2.y << ", " << v2.z << ")\n";
			auto len = glm::length(v1) / glm::length(v2);
			std::cout << "vectors q: " << len << std::endl;
			return 1;
		}
	}
	return 0;
}

int main()
{
	int res = test_cosvec3(1000000);
	printf("Finished with code %d\n", res);
	return res;
}