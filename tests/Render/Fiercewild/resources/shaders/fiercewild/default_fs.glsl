#version 330 core

in vec3 wFragPos;
in vec2 wTexCoords;
in vec3 wNormal;
in vec3 wForce;

out vec4 FragColor;

uniform sampler2D tex_d00; // diffuse
uniform sampler2D tex_s00; // specular
uniform vec3 viewPos; // camera position
uniform float glfwTime; // current time in seconds
uniform float lightStrength;

void main()
{
	vec3 lightPos = viewPos;
	// vec3 Kd = vec3(0.729411, 0.768627, 0.7843137); // zinc
	// vec3 Kd = vec3(0.7216, 0.451, 0.2); // copper
	// vec3 Kd = vec3(0.7686, 0.7921, 0.8078); // silver
	// vec3 Kd = vec3(0.6275, 0.698, 0.7765); // platinum
	// vec3 Kd = vec3(0.9451, 0.7333, 0.3687); // gold
	vec3 Kd = vec3(0.3);
	vec3 Ks = vec3(0.8);
	
	vec3 lightPtr = lightPos - wFragPos;
	vec3 lightDir = normalize(lightPtr); 
	vec3 viewDir = normalize(viewPos - wFragPos);
	
	vec3 specColor = vec3(texture(tex_d00, wTexCoords));
	vec3 diffColor = vec3(texture(tex_s00, wTexCoords));
	
	float diffIntensity = max(abs(dot(wNormal, lightDir)), 0.0);
	vec3 diffuse = diffIntensity * Kd * diffColor;
	
	vec3 reflectDir = reflect(-lightDir, wNormal);
	float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
	vec3 specular = specIntensity * Ks * specColor;
	
	vec3 ambient = diffColor * 0.07;
	
	float lightDist = length(lightPtr);
	float intensity = lightStrength / (lightDist * lightDist);
	
	vec3 final = (diffuse + specular) * intensity + ambient;
	
	FragColor = vec4(0.7 * final + vec3(0.3 * length(wForce)), 1.0);
}
