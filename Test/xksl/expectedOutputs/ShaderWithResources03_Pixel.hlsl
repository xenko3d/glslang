SamplerState ShaderMain_Sampler0;
Texture2D<float4> ShaderMain_Texture0;

void frag_main()
{
    float2 uv2 = 0.5f.xx;
    float4 color = ShaderMain_Texture0.Sample(ShaderMain_Sampler0, uv2);
}

void main()
{
    frag_main();
}
