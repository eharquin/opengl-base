#version 460 core

in vec2 vUV;

out vec4 FragColor;

uniform sampler2D texture0;

void main()
{
    FragColor = texture(texture0, vUV);
}