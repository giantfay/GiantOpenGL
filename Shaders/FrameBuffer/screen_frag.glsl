#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;

vec4 Inversion(vec4 color)
{
    return vec4(vec3(1.0f - color.rgb), 1.0f);
}

vec4 Grayscale(vec4 color)
{
    //float average = (color.r + color.g + color.b)/3.0f;
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return vec4(average, average, average, 1.0f);
}

const float offset = 1.0f/300.0f;

vec4 Kernel()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0f, offset),
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(offset, 0.0f),
        vec2(-offset, -offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
    );

    // sharpen
    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    
    // blur
    // float kernel[9] = float[](
    //     1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f,
    //     2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f,
    //     1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f
    // );

    // edge detection
    float kernel[9] = float[](
        1, 1, 1,
        1,-8, 1,
        1, 1, 1
    );

    vec3 sampColors[9];
    for(int i = 0; i < 9; ++i)
    {
        sampColors[i] = texture(texture1, TexCoords.st + offsets[i]).rgb;
    }

    vec3 color = vec3(0.0f);
    for(int i = 0; i < 9; ++i)
    {
        color += sampColors[i] * kernel[i];
    }

    return vec4(color, 1.0f);
}

void main()
{
    //FragColor = texture(texture1, TexCoords);
    //FragColor = Inversion(FragColor);
    //FragColor = Grayscale(FragColor);
    FragColor = Kernel();
}