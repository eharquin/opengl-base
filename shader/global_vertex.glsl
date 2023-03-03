#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vColor;

void main()
{
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	vec3 worldSpaceNormalNormalize = normalize(mat3(transpose(inverse(model))) * aNormal);
	vec3 worldSpacePosition = vec3(model * vec4(aPos, 1.0));

	// ambient light
	float ambientLightStrength = 0.2;
	vec3 ambient = ambientLightStrength * lightColor;

	// diffuse light
	vec3 lightDirectionNormalize = normalize(worldSpacePosition - lightPos);
	float diffuseLightStrenght = max(dot(worldSpaceNormalNormalize, -lightDirectionNormalize), 0.0);
	vec3 diffuse = diffuseLightStrenght * lightColor;

	// specular light
	int specularShininess = 64;
	float specularStrength = 0.5;
	vec3 viewDirectionNormalize = normalize(worldSpacePosition - viewPos);
	vec3 reflectLightDirectionNormalize = reflect(lightDirectionNormalize, worldSpaceNormalNormalize);
	float specularLightStrength = pow(max(dot(-viewDirectionNormalize, reflectLightDirectionNormalize), 0.0), specularShininess);
	vec3 specular = specularStrength * specularLightStrength * lightColor;

    vColor = (ambient + diffuse + specular) * objectColor;
}