#version 310 es
precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = inColor;
    gl_Position = vec4(inPosition, 1.0);
}
