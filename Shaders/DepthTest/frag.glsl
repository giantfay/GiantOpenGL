#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform float near;
uniform float far;

float GetViewSpaceDepth(float fragZ)
{
    float ndcZ = 2.0f*fragZ - 1.0f;
    return 2*far*near/(far + near - ndcZ*(far - near));
}

void main()
{
    float d = GetViewSpaceDepth(gl_FragCoord.z)/far;
    FragColor = vec4(vec3(d), 1.0);;
}