struct mat_comp
{
    float3x3 rotation;
    float3 translation;
    float3 scale;
};

cbuffer UBO : register(b0, space0)
{
    float4 _167_camera_pos : packoffset(c0);
    float4 _167_model_mat_v1 : packoffset(c1);
    float4 _167_model_mat_v2 : packoffset(c2);
    float4 _167_model_mat_v3 : packoffset(c3);
    float4 _167_model_mat_v4 : packoffset(c4);
    float2 _167_mouse : packoffset(c5);
    float _167_blend : packoffset(c5.z);
    float3 _167_glow : packoffset(c6);
};


static float4 gl_Position;
static float3 out_glow;
static float3 p;
static float3 p2;
static float3 vp;
static float3 vn;
static float3 n;
static float3 n2;
static float2 vt;
static float2 t;

struct SPIRV_Cross_Input
{
    float3 p : TEXCOORD0;
    float3 n : TEXCOORD1;
    float3 p2 : TEXCOORD2;
    float3 n2 : TEXCOORD3;
    float2 t : TEXCOORD4;
};

struct SPIRV_Cross_Output
{
    float3 vp : TEXCOORD0;
    float3 vn : TEXCOORD1;
    float2 vt : TEXCOORD2;
    float3 out_glow : TEXCOORD3;
    float4 gl_Position : SV_Position;
};

float4 clamp_to_fixed_point(inout float4 val)
{
    float _step = 0.03125f;
    val = round(val / _step.xxxx) * _step.xxxx;
    return val;
}

mat_comp decomp_mat(float4x4 _matrix)
{
    mat_comp components;
    components.translation = _matrix[3].xyz;
    components.scale = float3(length(_matrix[0].xyz), length(_matrix[1].xyz), length(_matrix[2].xyz));
    components.rotation = float3x3(float3(_matrix[0].xyz / components.scale.x.xxx), float3(_matrix[1].xyz / components.scale.y.xxx), float3(_matrix[2].xyz / components.scale.z.xxx));
    return components;
}

float3 clamp_v3_to_fixed_point(inout float3 val)
{
    float _step = 0.03125f;
    val = round(val / _step.xxx) * _step.xxx;
    return val;
}

float4x4 rx(float r)
{
    return float4x4(float4(1.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, cos(r), sin(r), 0.0f), float4(0.0f, -sin(r), cos(r), 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f));
}

float4x4 ry(float r)
{
    return float4x4(float4(cos(r), 0.0f, -sin(r), 0.0f), float4(0.0f, 1.0f, 0.0f, 0.0f), float4(sin(r), 0.0f, cos(r), 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f));
}

void main_inner()
{
    out_glow = _167_glow;
    float4 param = _167_model_mat_v1;
    float4 _177 = clamp_to_fixed_point(param);
    float4 param_1 = _167_model_mat_v2;
    float4 _181 = clamp_to_fixed_point(param_1);
    float4 param_2 = _167_model_mat_v3;
    float4 _185 = clamp_to_fixed_point(param_2);
    float4 param_3 = _167_model_mat_v4;
    float4 _190 = clamp_to_fixed_point(param_3);
    float4x4 model_mat = float4x4(float4(_177), float4(_181), float4(_185), float4(_190));
    float4x4 param_4 = model_mat;
    mat_comp model = decomp_mat(param_4);
    float3 vp_in = mul(model.scale * lerp(p, p2, _167_blend.xxx), model.rotation) + model.translation;
    float3 param_5 = vp_in;
    float3 _240 = clamp_v3_to_fixed_point(param_5);
    vp = _240;
    vn = mul(lerp(n, n2, _167_blend.xxx), model.rotation);
    vt = t;
    float3 param_6 = _167_camera_pos.xyz;
    float3 _263 = clamp_v3_to_fixed_point(param_6);
    float3 camera_pos_xyz = _263;
    float param_7 = -_167_mouse.y;
    float param_8 = -_167_mouse.x;
    gl_Position = mul(float4(vp - camera_pos_xyz, 1.0f), mul(ry(param_8), mul(rx(param_7), float4x4(float4(1.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, _167_camera_pos.w, 0.0f, 0.0f), float4(0.0f, 0.0f, 1.0f, 1.0f), float4(0.0f, 0.0f, -2.0f, 0.0f)))));
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    p = stage_input.p;
    p2 = stage_input.p2;
    n = stage_input.n;
    n2 = stage_input.n2;
    t = stage_input.t;
    main_inner();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.out_glow = out_glow;
    stage_output.vp = vp;
    stage_output.vn = vn;
    stage_output.vt = vt;
    return stage_output;
}
