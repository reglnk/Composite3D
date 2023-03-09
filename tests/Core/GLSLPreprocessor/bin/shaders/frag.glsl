#version 330 core

in vec3 wFragPos;
in vec2 wTexCoords;
in vec3 wNormal;

out vec4 FragColor;

uniform sampler2D tex_d00;
uniform sampler2D tex_s00;
uniform vec3 viewPos;
uniform float Time;
uniform float lightStrength;

#pragma cm3d_include "inc/header.glsl"

void main()
{
	FragColor = vec4(0.7, 0.3, 0.7, 1.0) * lightStrength / dot(viewPos, viewPos);
}
