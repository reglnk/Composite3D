#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoords;
layout (location = 2) in vec3 vNormal;

out vec3 wFragPos;
out vec2 wTexCoords;
out vec3 wNormal;
out vec3 wForce;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

// The force is encoded with last 1 byte of each float
float decodef(float u)
{
	c = floatBitsToInt(u);
	c <<= 24;
	float val = (c >= 0) ?
		float(c >> 24) :
		float(-(-c >> 24));
	return val * (1.0 / 128.0);
}

vec3 decodev(vec3 c)
{
	return vec3(decodef(c.x), decodef(c.y), decodef(c.z));
}

void main()
{
	wTexCoords = vTexCoords;
	wNormal = vec3(mat3(transform) * vNormal);
	wFragPos = vec3(transform * vec4(vPosition, 1.0));

	wForce = decodev(vPosition);
	
	gl_Position = projection * view * transform * vec4(vPosition, 1.0);
}
