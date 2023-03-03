#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light; 

uniform vec3 viewPos;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoords;

out vec4 FragColor;

void main()
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vTexCoords));

    // diffuse
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(vPos - light.position);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vTexCoords));

    // specular
	vec3 viewDir = normalize(viewPos - vPos);
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));

    // emission
    vec3 emission = vec3(0.0);
    if(vec3(texture(material.specular, vTexCoords)) == vec3(0.0, 0.0, 0.0))
    {
        emission = vec3(texture(material.emission, vTexCoords));
    }

    
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}