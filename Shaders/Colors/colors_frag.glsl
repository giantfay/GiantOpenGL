#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;

struct Light //spot light
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

uniform Light light;

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

uniform Material material;

void main()
{
    vec3 ambient = texture(material.diffuseMap, TexCoord).rgb * light.ambient;

    vec3 lightDir = normalize(light.position - FragPos);

    float theta = dot(lightDir, -light.direction);
    float intensity = clamp((theta - light.outerCutOff)/(light.cutOff - light.outerCutOff), 0.0f, 1.0f);
    
    vec3 normal = normalize(Normal);
    float distanceToLight = length(light.position - FragPos);

    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuseMap, TexCoord).rgb;
    diffuse *= intensity;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);
    vec3 specular = texture(material.specularMap, TexCoord).rgb * spec * light.specular;
    specular *= intensity;

    float attenuation = 1.0f/(light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
    // ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}