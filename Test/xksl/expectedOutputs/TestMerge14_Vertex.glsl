#version 450

struct ShaderBase_sBase2
{
    int i;
};

struct VS_STREAMS
{
    int streamBase_id0;
    int streamA_id1;
};

layout(location = 0) out int VS_OUT_streamBase;
layout(location = 1) out int VS_OUT_streamA;

void ShaderBase_ComputeBase(inout VS_STREAMS _streams, ShaderBase_sBase2 s1, ShaderBase_sBase2 s2)
{
    _streams.streamBase_id0 = s1.i + s2.i;
}

void ShaderA_Compute(inout VS_STREAMS _streams, ShaderBase_sBase2 s)
{
    ShaderBase_sBase2 sb1 = ShaderBase_sBase2(s.i);
    ShaderBase_sBase2 sb2 = ShaderBase_sBase2(s.i);
    ShaderBase_sBase2 param = sb1;
    ShaderBase_sBase2 param_1 = sb2;
    ShaderBase_ComputeBase(_streams, param, param_1);
    _streams.streamA_id1 = 1;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0, 0);
    ShaderBase_sBase2 s = ShaderBase_sBase2(1);
    ShaderBase_sBase2 param = s;
    ShaderA_Compute(_streams, param);
    VS_OUT_streamBase = _streams.streamBase_id0;
    VS_OUT_streamA = _streams.streamA_id1;
}

