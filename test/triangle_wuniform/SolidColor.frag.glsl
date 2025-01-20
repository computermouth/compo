#version 310 es
precision mediump float;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec4 fakeOutParam;

layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 0) uniform UBO {
    vec4 addColor;
    vec4 subColor;
};

void main()
{
    outColor = fragColor - subColor + addColor;
}
