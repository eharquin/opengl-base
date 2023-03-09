#version 460 core

struct Material
{
    sampler2D texture_diffuse[16];
    sampler2D texture_specular[16];
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
#define NR_POINT_LIGHTS 4
uniform PointLight pointLight[NR_POINT_LIGHTS];

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
uniform bool useSpotLight; 

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
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse[0], vTexCoords));

    // diffuse
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse[0], vTexCoords));

    // specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular[0], vTexCoords));

    return ambient + diffuse + specular;
}

// vec3 need to be normalize
vec3 ComputePointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse[0], vTexCoords));

    // diffuse
    vec3 lightDir = normalize(vPos - light.position);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse[0], vTexCoords));

    // specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular[0], vTexCoords));

    // attenuation
    float distanceToLight = length(vec3(light.position) - vPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
    
    return attenuation * (ambient + diffuse + specular);
}

// vec3 need to be normalize
vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 viewDirection)
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse[0], vTexCoords));

    // diffuse
    vec3 lightDir = normalize(vPos - light.position);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse[0], vTexCoords));

    // specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular[0], vTexCoords));

    // attenuation
    float distanceToLight = length(vec3(light.position) - vPos);
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
    vec3 viewDirectionNormalized = normalize(viewPos - vPos);
    
    vec3 result = vec3(0.0);
    result += ComputeDirectionalLight(directionalLight, normal, viewDirectionNormalized);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += ComputePointLight(pointLight[i], normal, viewDirectionNormalized);

    if(useSpotLight)
        result += ComputeSpotLight(spotLight, normal, viewDirectionNormalized);

    // emission
//    vec3 emission = vec3(0.0);
//    if(vec3(texture(material.specular, vTexCoords)) == vec3(0.0, 0.0, 0.0))
//    {
//        emission = vec3(texture(material.emission, vTexCoords));
//    }



    FragColor = vec4(result, 1.0);
}