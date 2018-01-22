struct PS_STREAMS
{
    float2 Position_id0;
    float4 ColorTarget_id1;
};

static float2 PS_OUT_Position;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Output
{
    float2 PS_OUT_Position : SV_Target1;
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float4 ShaderMain_Compute(inout PS_STREAMS _streams)
{
    _streams.Position_id0 = 1.0f.xx;
    return float4(_streams.Position_id0, 0.0f, 0.0f);
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xx, 0.0f.xxxx };
    float4 _12 = ShaderMain_Compute(_streams);
    _streams.ColorTarget_id1 = _12 + float4(_streams.Position_id0, 0.0f, 1.0f);
    PS_OUT_Position = _streams.Position_id0;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_Position = PS_OUT_Position;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
