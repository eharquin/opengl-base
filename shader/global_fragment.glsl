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
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
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

    // attenuation
    float distanceToLight = length(vec3(light.position) - vPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
    
    float theta = dot(lightDir, normalize(light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutOff) / epsilon);

    vec3 result;
    if(theta > light.outerCutOff)
    {
        diffuse  *= intensity;
        specular *= intensity;

        result = attenuation * (ambient + diffuse + specular + emission);
    }
    else
    {
        result = ambient;
    }

    FragColor = vec4(result, 1.0);
}