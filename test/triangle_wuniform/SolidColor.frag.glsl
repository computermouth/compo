#version 310 es
precision mediump float;

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 0) uniform UBO {
    vec3 subColor;
};

void main()
{
    outColor = fragColor - vec4(subColor, 0);
}
