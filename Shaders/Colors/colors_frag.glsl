#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

out vec4 FragColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

uniform vec3 objColor;
uniform vec3 lightColor;

void main()
{
    vec3 ambient = ambientStrength*lightColor;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);

    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    vec3 viewDir = normalize( - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(result, 1.0f);
}