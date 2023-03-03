#version 460 core

uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 vNormal;
in vec3 vPos;
in vec3 vLightPos; // in view space

out vec4 FragColor;

void main()
{
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(vPos - vLightPos);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
	vec3 viewDir = normalize(-vPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;  
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}