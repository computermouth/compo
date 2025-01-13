cbuffer UBO : register(b1, space0)
{
    int _40_light_count : packoffset(c0);
    float3 _40_lights[64] : packoffset(c1);
};

Texture2D<float4> s : register(t0, space0);
SamplerState _s_sampler : register(s0, space0);

static float4 fragColor;
static float2 vt;
static float3 vn;
static float3 vp;
static float3 out_glow;

struct SPIRV_Cross_Input
{
    float3 vp : TEXCOORD0;
    float3 vn : TEXCOORD1;
    float2 vt : TEXCOORD2;
    float3 out_glow : TEXCOORD3;
};

struct SPIRV_Cross_Output
{
    float4 fragColor : SV_Target0;
};

void main_inner()
{
    fragColor = s.Sample(_s_sampler, vt);
    float3 vl = 0.0f.xxx;
    for (int i = 0; i < _40_light_count; i += 2)
    {
        vl += (_40_lights[i + 1] * (max(dot(vn, normalize(_40_lights[i] - vp)), 0.0f) * (1.0f / pow(length(_40_lights[i] - vp), 2.0f))));
    }
    float3 p = pow(vl, 0.75f.xxx);
    if (!isnan(out_glow.x))
    {
        fragColor.x = out_glow.x;
        fragColor.y = out_glow.y;
        fragColor.z = out_glow.z;
        return;
    }
    float4 _106 = fragColor;
    float3 _117 = floor(((_106.xyz * p) * 16.0f) + 0.5f.xxx) / 16.0f.xxx;
    fragColor.x = _117.x;
    fragColor.y = _117.y;
    fragColor.z = _117.z;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    vt = stage_input.vt;
    vn = stage_input.vn;
    vp = stage_input.vp;
    out_glow = stage_input.out_glow;
    main_inner();
    SPIRV_Cross_Output stage_output;
    stage_output.fragColor = fragColor;
    return stage_output;
}
