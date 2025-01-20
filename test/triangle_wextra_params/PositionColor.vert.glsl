#version 310 es
precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec4 fakeInParam;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fakeOutParam;

void main()
{
    fakeOutParam = fakeInParam;
    fragColor = inColor;
    gl_Position = vec4(inPosition, 1.0);
}
