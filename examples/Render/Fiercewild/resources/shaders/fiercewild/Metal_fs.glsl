#version 330 core

in vec3 wFragPos;
in vec2 wTexCoords;
in vec3 wNormal;

out vec4 FragColor;

uniform vec3 uColAmbient;
uniform vec3 uColDiffuse;
uniform vec3 uColSpecular;
uniform vec3 uColEmissive;
uniform vec3 uColTransparent;

uniform float uPropOpacity;
uniform float uPropRoughness;
uniform float uPropMetalness;

uniform sampler2D uTexBaseColor;
uniform sampler2D uTexDiffuse;
uniform sampler2D uTexSpecular;

void main()
{
	vec3 diffuse = vec3(texture(uTexBaseColor, wTexCoords));
	FragColor = vec4(diffuse, 1.0);
}

