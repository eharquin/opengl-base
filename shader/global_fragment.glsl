#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;


struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight directionalLight;

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLight;

struct SpotLight
{
    vec3 direction;
    vec3 position;
    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight spotLight;

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


// vec3 need to be normalize
vec3 ComputeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
    vec3 result;

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vTexCoords));

    // diffuse
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vTexCoords));

    // specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));

    return ambient + diffuse + specular;
}

// vec3 need to be normalize
vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vTexCoords));

    // diffuse
    vec3 lightDir = normalize(fragPos - light.position);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vTexCoords));

    // specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));

    // attenuation
    float distanceToLight = length(vec3(light.position) - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
    
    return attenuation * (ambient + diffuse + specular);
}

// vec3 need to be normalize
vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vTexCoords));

    // diffuse
    vec3 lightDir = normalize(fragPos - light.position);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vTexCoords));

    // specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));

    // attenuation
    float distanceToLight = length(vec3(light.position) - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);

    float theta = dot(lightDir, normalize(light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutOff) / epsilon);

    vec3 result;
    if(theta > light.outerCutOff)
    {
        diffuse  *= intensity;
        specular *= intensity;

        result = attenuation * (ambient + diffuse + specular);
    }
    else
    {
        result = ambient;
    }
    
    return result;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDirectionNormalized = normalize(viewDirection);

    
    vec3 result(0.0);



    result += ComputeDirectionalLight(directionalLight, normal, viewDirection

    // emission
//    vec3 emission = vec3(0.0);
//    if(vec3(texture(material.specular, vTexCoords)) == vec3(0.0, 0.0, 0.0))
//    {
//        emission = vec3(texture(material.emission, vTexCoords));
//    }



    FragColor = vec4(result, 1.0);
}