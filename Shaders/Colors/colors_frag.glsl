#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight dirLight;

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

#define POINT_LIGHT_NUM 4
uniform PointLight pointLights[POINT_LIGHT_NUM];

struct SpotLight //spot light
{
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

uniform SpotLight light;

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

uniform Material material;

vec3 CalcDirectionalLightColor(DirectionalLight light, Material mat, vec3 normal, vec3 viewDir)
{
    //ambient
    vec3 albedo = texture(mat.diffuseMap, TexCoord).rgb;
    vec3 ambient = albedo * light.ambient;
    //diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, viewDir), 0.0f);
    vec3 diffuse = diff * albedo * light.diffuse;
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), mat.shininess);
    vec3 specular = spec * texture(mat.specularMap, TexCoord).rgb * light.specular;

    return ambient + diffuse + specular;
}

vec3 CalcPointLightColor(PointLight light, Material mat, vec3 normal, vec3 viewDir)
{
    //ambient
    vec3 albedo = texture(mat.diffuseMap, TexCoord).rgb;
    vec3 ambient = albedo * light.ambient;
    //diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(lightDir, viewDir), 0.0f);
    vec3 diffuse = diff * albedo * light.diffuse;
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), mat.shininess);
    vec3 specular = spec * texture(mat.specularMap, TexCoord).rgb * light.specular;

    float distanceToLight = length(light.position - FragPos);
    float attenuation = 1.0f/(light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirectionalLightColor(dirLight, material, normal, viewDir);
    for(int i = 0; i < POINT_LIGHT_NUM; ++i)
    {
        result += CalcPointLightColor(pointLights[i], material, normal, viewDir);
    } 
    
    FragColor = vec4(result, 1.0f);
    // vec3 ambient = texture(material.diffuseMap, TexCoord).rgb * light.ambient;

    // vec3 lightDir = normalize(light.position - FragPos);

    // float theta = dot(lightDir, -light.direction);
    // float intensity = clamp((theta - light.outerCutOff)/(light.cutOff - light.outerCutOff), 0.0f, 1.0f);
    
    // vec3 normal = normalize(Normal);
    // float distanceToLight = length(light.position - FragPos);

    // float diff = max(dot(normal, lightDir), 0.0f);
    // vec3 diffuse = diff * light.diffuse * texture(material.diffuseMap, TexCoord).rgb;
    // diffuse *= intensity;

    // vec3 viewDir = normalize(viewPos - FragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);
    // vec3 specular = texture(material.specularMap, TexCoord).rgb * spec * light.specular;
    // specular *= intensity;

    // float attenuation = 1.0f/(light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
    // // ambient *= attenuation;
    // diffuse *= attenuation;
    // specular *= attenuation;

    // vec3 result = ambient + diffuse + specular;
    // FragColor = vec4(result, 1.0f);
}