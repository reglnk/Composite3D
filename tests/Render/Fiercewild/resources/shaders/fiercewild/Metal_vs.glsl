#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoords;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out vec3 wFragPos;
out vec2 wTexCoords;
out vec3 wNormal;

uniform mat4 uView;
uniform mat4 uNodeTransform;

void main()
{
	wTexCoords = vTexCoords;
	wNormal = vec3(mat3(uNodeTransform) * vNormal);
	wFragPos = vec3(uNodeTransform * vec4(vPosition, 1.0));

	gl_Position = uView * uNodeTransform * vec4(vPosition.xyz, 1.0);
}

