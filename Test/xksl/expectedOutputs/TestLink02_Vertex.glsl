#version 450

struct VS_STREAMS
{
    vec4 s_in_id0;
    vec4 s_int_id1;
};

layout(std140) uniform PerMaterial
{
    vec4 ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__scale;
    vec4 ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__offset;
    vec4 ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__aCol;
    vec2 ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__uv2;
} PerMaterial_var;

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__Texture0ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__Sampler0;

layout(location = 0) in vec4 VS_IN_s_in;
layout(location = 0) out vec4 VS_OUT_s_int;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.s_in_id0 = VS_IN_s_in;
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__Texture0ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__Sampler0, PerMaterial_var.ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__uv2);
    _streams.s_int_id1 = _streams.s_in_id0 + PerMaterial_var.ShaderMain_Material_DiffuseMap_Material_TextureScale_Material_TextureOffset__scale;
    VS_OUT_s_int = _streams.s_int_id1;
}

