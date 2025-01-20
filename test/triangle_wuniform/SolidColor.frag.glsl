#version 310 es
precision mediump float;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec4 fakeOutParam;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = fragColor * fakeOutParam;
}
