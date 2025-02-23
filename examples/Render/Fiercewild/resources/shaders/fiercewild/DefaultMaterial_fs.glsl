#version 330 core

in vec3 wFragPos;
in vec2 wTexCoords;
in vec3 wNormal;
in vec3 wForce;

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

