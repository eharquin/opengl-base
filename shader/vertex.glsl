#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 vUV;

uniform mat4 transform;

void main()
{
   gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
   vUV = aUV;
}