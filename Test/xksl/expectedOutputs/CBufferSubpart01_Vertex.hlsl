struct VS_STREAMS
{
    int aStream_id0;
};

cbuffer PerLighting
{
    int ShaderMain_var7;
    int ShaderMain_var8;
    float4 ShaderMain__padding__Default;
    int ShaderMain_var3;
    int ShaderMain_var4;
    float4 ShaderMain__padding__subpart2;
    int ShaderMain_var1;
    int ShaderMain_var2;
    float4 ShaderMain__padding__subpart1;
    int ShaderMain_var9;
    int ShaderMain_var10;
    int ShaderMain_var5;
    int ShaderMain_var6;
    float4 ShaderMain__padding__subpart3;
};

static int VS_OUT_aStream;

struct SPIRV_Cross_Output
{
    int VS_OUT_aStream : TEXCOORD0;
};

void vert_main()
{
    VS_STREAMS _streams = { 0 };
    _streams.aStream_id0 = (((ShaderMain_var1 + ShaderMain_var3) + ShaderMain_var5) + ShaderMain_var7) + ShaderMain_var9;
    VS_OUT_aStream = _streams.aStream_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream = VS_OUT_aStream;
    return stage_output;
}
