#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vNormal;
out vec3 vPos;
out vec3 vLightPos; // in view space

void main()
{
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	vNormal = mat3(transpose(inverse(view * model))) * aNormal;
	vPos = vec3(view * model * vec4(aPos, 1.0));
	vLightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
}