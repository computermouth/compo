#version 310 es

precision highp float;

// Input vertex positions & normals and blend vertex positions & normals
layout(location = 0) in vec3 p;
layout(location = 1) in vec3 n;
layout(location = 2) in vec3 p2;
layout(location = 3) in vec3 n2;
// Input UV coords
layout(location = 4) in vec2 t;

layout(set = 0, binding = 0) uniform UBO {
    // Camera position (x, y, z) and aspect ratio (w)
    vec4 camera_pos;

    // Model scale, rotation, translation(pos) v4 -> matrix
    vec4 model_mat_v1;
    vec4 model_mat_v2;
    vec4 model_mat_v3;
    vec4 model_mat_v4;

    // Mouse rotation yaw (x), pitch (y)
    vec2 mouse;

    // Blend factor between the two vertex positions
    float blend;

    // Use a static multiplier to light, instead
    // of dynamic lighting
    vec3 glow;
};

// Vertex positions, normals and UV coords for the fragment shader
layout(location = 0) out vec3 vp;
layout(location = 1) out vec3 vn;
layout(location = 2) out vec2 vt;
layout(location = 3) out vec3 out_glow;

// Generate a rotation Matrix around the x,y,z axis;
// Used for model rotation and camera yaw
mat4 rx(float r) {
    return mat4(
        1, 0, 0, 0,
        0, cos(r), sin(r), 0,
        0, -sin(r), cos(r), 0,
        0, 0, 0, 1
    );
}

mat4 ry(float r) {
    return mat4(
        cos(r), 0, -sin(r), 0,
        0, 1, 0, 0,
        sin(r), 0, cos(r), 0,
        0, 0, 0, 1
    );
}

mat4 rz(float r) {
    return mat4(
        cos(r), sin(r), 0, 0,
        -sin(r), cos(r), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

struct mat_comp {
    mat3 rotation;
    vec3 translation;
    vec3 scale;
};

mat_comp decomp_mat(mat4 matrix) {
    mat_comp components;

    components.translation = vec3(matrix[3].xyz);

    components.scale = vec3(
        length(matrix[0].xyz),
        length(matrix[1].xyz),
        length(matrix[2].xyz)
    );

    components.rotation = mat3(
        matrix[0].xyz / components.scale.x,
        matrix[1].xyz / components.scale.y,
        matrix[2].xyz / components.scale.z
    );

    return components;
}

vec4 clamp_to_fixed_point(vec4 val) {

    // 1 / 32
    // float step = 0.00390625;
    float step = 1.0/32.0;

    val = round(val / vec4(step)) * vec4(step);

    return val;
}

vec3 clamp_v3_to_fixed_point(vec3 val) {

    // 1 / 32
    // float step = 0.00390625;
    float step = 1.0/32.0;

    val = round(val / vec3(step)) * vec3(step);

    // vec3 step_v3 = vec3(step * sin(val.x), step * sin(val.y), step * sin(val.z));
    // val = round(val / step_v3) * step_v3;

    return val;
}

// todo 
// use
// uint packHalf2x16(vec2 v); 
// vec2 unpackHalf2x16(uint v);
// to truncate floats instead of clamping??

void main(void) {
    out_glow = glow;

    // scale, rotation, translation matrix
    mat4 model_mat = mat4 (
        clamp_to_fixed_point(model_mat_v1),
        clamp_to_fixed_point(model_mat_v2),
        clamp_to_fixed_point(model_mat_v3),
        clamp_to_fixed_point(model_mat_v4)
    );

    mat_comp model = decomp_mat(model_mat);
 
    // Mix vertex positions, rotate using mat3, and add the translation
    vec3 vp_in = model.rotation * (model.scale * mix(p, p2, blend)) + model.translation;

    // what if the step was also like
    // vec3(step + sin(val.x), step + sin(val.y), step + sin(val.z))
    vp = clamp_v3_to_fixed_point(vp_in);

    // Mix normals
    vn = model.rotation * mix(n, n2, blend);

    // UV coords are handed over to the fragment shader as is
    vt = t;

    vec3 camera_pos_xyz = clamp_v3_to_fixed_point(camera_pos.xyz);

    // Final vertex position is transformed by the projection matrix,
    // rotated around mouse yaw/pitch and offset by the camera position
    // We use a FOV of 90, so the matrix[0] and [5] are conveniently 1.
    // (1 / Math.tan((90/180) * Math.PI / 2) === 1)
    gl_Position = mat4(
        1, 0, 0, 0,
        0, camera_pos.w, 0, 0,
        0, 0, 1, 1,
        0, 0, -2, 0
    ) * // projection
    rx(-mouse.y) * ry(-mouse.x) *
    vec4(vp - camera_pos_xyz, 1.0);
}
