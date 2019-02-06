#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct LightDirectional_DirectionalLightData
{
    vec3 DirectionWS;
    vec3 Color;
};

struct LightPoint_PointLightDataInternal
{
    vec3 PositionWS;
    float InvSquareRadius;
    vec3 Color;
};

struct LightSpot_SpotLightDataInternal
{
    vec3 PositionWS;
    vec3 DirectionWS;
    vec3 AngleOffsetAndInvSquareRadius;
    vec3 Color;
};

struct VS_STREAMS
{
    vec3 lightPositionWS_id0;
    vec3 lightDirectionWS_id1;
    vec3 lightColor_id2;
    vec3 lightColorNdotL_id3;
    float NdotL_id4;
    float lightDirectAmbientOcclusion_id5;
    vec3 normalWS_id6;
    vec3 shadowColor_id7;
    float thicknessWS_id8;
    vec4 PositionWS_id9;
    uvec2 lightData_id10;
    int lightIndex_id11;
    vec4 ShadingPosition_id12;
    vec4 ScreenPosition_id13;
};

layout(std140) uniform PerView
{
    float o1S2C0_Camera_NearClipPlane;
    float o1S2C0_Camera_FarClipPlane;
    vec2 o1S2C0_Camera_ZProjection;
    vec2 o1S2C0_Camera_ViewSize;
    float o1S2C0_Camera_AspectRatio;
    vec4 o0S2C0_LightDirectionalGroup_padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S2C0_LightDirectionalGroup_Lights[8];
    int o0S2C0_DirectLightGroupPerView_LightCount;
    float o1S2C0_LightClustered_ClusterDepthScale;
    float o1S2C0_LightClustered_ClusterDepthBias;
    vec2 o1S2C0_LightClustered_ClusterStride;
} PerView_var;

uniform samplerBuffer LightClusteredPointGroup_PointLights;
uniform usamplerBuffer LightClustered_LightIndices;
uniform samplerBuffer LightClusteredSpotGroup_SpotLights;
uniform usampler3D SPIRV_Cross_CombinedLightClustered_LightClustersSPIRV_Cross_DummySampler;

in float VS_IN_LIGHTDIRECTAMBIENTOCCLUSION;
in vec3 VS_IN_NORMALWS;
in vec4 VS_IN_POSITION_WS;
in vec4 VS_IN_SV_Position;
in vec4 VS_IN_SCREENPOSITION;
out vec4 VS_OUT_ScreenPosition;

void o0S2C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S2C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S2C0_DirectLightGroupPerView_GetLightCount()
{
    return PerView_var.o0S2C0_DirectLightGroupPerView_LightCount;
}

void o0S2C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id2 = PerView_var.o0S2C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id1 = -PerView_var.o0S2C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

vec3 o0S2C0_ShadowGroup_ComputeShadow(inout VS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id8 = 0.0;
    return vec3(1.0);
}

void o0S2C0_DirectLightGroup_PrepareDirectLight(inout VS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S2C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id4 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id1), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    vec3 _52 = o0S2C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id7 = _52;
    _streams.lightColorNdotL_id3 = ((_streams.lightColor_id2 * _streams.shadowColor_id7) * _streams.NdotL_id4) * _streams.lightDirectAmbientOcclusion_id5;
}

void o1S2C0_LightClustered_PrepareLightData(inout VS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id12.z;
    float depth = PerView_var.o1S2C0_Camera_ZProjection.y / (projectedDepth - PerView_var.o1S2C0_Camera_ZProjection.x);
    vec2 texCoord = vec2(_streams.ScreenPosition_id13.x + 1.0, 1.0 - _streams.ScreenPosition_id13.y) * 0.5;
    int slice = int(max(log2((depth * PerView_var.o1S2C0_LightClustered_ClusterDepthScale) + PerView_var.o1S2C0_LightClustered_ClusterDepthBias), 0.0));
    _streams.lightData_id10 = uvec2(texelFetch(SPIRV_Cross_CombinedLightClustered_LightClustersSPIRV_Cross_DummySampler, ivec4(ivec2(texCoord * PerView_var.o1S2C0_LightClustered_ClusterStride), slice, 0).xyz, ivec4(ivec2(texCoord * PerView_var.o1S2C0_LightClustered_ClusterStride), slice, 0).w).xy);
    _streams.lightIndex_id11 = int(_streams.lightData_id10.x);
}

void o1S2C0_LightClusteredPointGroup_PrepareDirectLights(inout VS_STREAMS _streams)
{
    o1S2C0_LightClustered_PrepareLightData(_streams);
}

int o1S2C0_LightClusteredPointGroup_GetMaxLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y & 65535u);
}

int o1S2C0_LightClusteredPointGroup_GetLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y & 65535u);
}

float o1S2C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o1S2C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S2C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S2C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, vec3 position, out vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    float lightInvSquareRadius = light.InvSquareRadius;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S2C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S2C0_LightPoint_ProcessLight(inout VS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _199 = o1S2C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _199;
    _streams.lightPositionWS_id0 = light.PositionWS;
    _streams.lightColor_id2 = light.Color * attenuation;
    _streams.lightDirectionWS_id1 = lightVectorNorm;
}

void o1S2C0_LightClusteredPointGroup_PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id11).x);
    _streams.lightIndex_id11++;
    vec4 pointLight1 = texelFetch(LightClusteredPointGroup_PointLights, realLightIndex * 2);
    vec4 pointLight2 = texelFetch(LightClusteredPointGroup_PointLights, (realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S2C0_LightPoint_ProcessLight(_streams, param);
}

vec3 o1S2C0_ShadowGroup_ComputeShadow(inout VS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id8 = 0.0;
    return vec3(1.0);
}

void o1S2C0_DirectLightGroup_PrepareDirectLight(inout VS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S2C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id4 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id1), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    vec3 _115 = o1S2C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id7 = _115;
    _streams.lightColorNdotL_id3 = ((_streams.lightColor_id2 * _streams.shadowColor_id7) * _streams.NdotL_id4) * _streams.lightDirectAmbientOcclusion_id5;
}

void o2S2C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S2C0_LightClusteredSpotGroup_GetMaxLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y >> uint(16));
}

int o2S2C0_LightClusteredSpotGroup_GetLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y >> uint(16));
}

float o2S2C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o2S2C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S2C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S2C0_LightUtil_GetAngleAttenuation(vec3 lightVector, vec3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0, 1.0);
    attenuation *= attenuation;
    return attenuation;
}

float o2S2C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, vec3 position, inout vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    vec3 lightAngleOffsetAndInvSquareRadius = light.AngleOffsetAndInvSquareRadius;
    vec2 lightAngleAndOffset = lightAngleOffsetAndInvSquareRadius.xy;
    float lightInvSquareRadius = lightAngleOffsetAndInvSquareRadius.z;
    vec3 lightDirection = -light.DirectionWS;
    float attenuation = 1.0;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S2C0_LightUtil_GetDistanceAttenuation(param, param_1);
    vec3 param_2 = lightVectorNorm;
    vec3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S2C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S2C0_LightSpot_ProcessLight(inout VS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _482 = o2S2C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _482;
    _streams.lightColor_id2 = light.Color * attenuation;
    _streams.lightDirectionWS_id1 = lightVectorNorm;
}

void o2S2C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id11).x);
    _streams.lightIndex_id11++;
    vec4 spotLight1 = texelFetch(LightClusteredSpotGroup_SpotLights, realLightIndex * 4);
    vec4 spotLight2 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 1);
    vec4 spotLight3 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 2);
    vec4 spotLight4 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 3);
    LightSpot_SpotLightDataInternal spotLight;
    spotLight.PositionWS = spotLight1.xyz;
    spotLight.DirectionWS = spotLight2.xyz;
    spotLight.AngleOffsetAndInvSquareRadius = spotLight3.xyz;
    spotLight.Color = spotLight4.xyz;
    LightSpot_SpotLightDataInternal param = spotLight;
    o2S2C0_LightSpot_ProcessLight(_streams, param);
}

vec3 o2S2C0_ShadowGroup_ComputeShadow(inout VS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id8 = 0.0;
    return vec3(1.0);
}

void o2S2C0_DirectLightGroup_PrepareDirectLight(inout VS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S2C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id4 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id1), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    vec3 _386 = o2S2C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id7 = _386;
    _streams.lightColorNdotL_id3 = ((_streams.lightColor_id2 * _streams.shadowColor_id7) * _streams.NdotL_id4) * _streams.lightDirectAmbientOcclusion_id5;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, vec3(0.0), vec3(0.0), 0.0, vec4(0.0), uvec2(0u), 0, vec4(0.0), vec4(0.0));
    _streams.lightDirectAmbientOcclusion_id5 = VS_IN_LIGHTDIRECTAMBIENTOCCLUSION;
    _streams.normalWS_id6 = VS_IN_NORMALWS;
    _streams.PositionWS_id9 = VS_IN_POSITION_WS;
    _streams.ShadingPosition_id12 = VS_IN_SV_Position;
    _streams.ScreenPosition_id13 = VS_IN_SCREENPOSITION;
    o0S2C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S2C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S2C0_DirectLightGroupPerView_GetLightCount();
    int i = 0;
    int param;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param = i;
        o0S2C0_DirectLightGroup_PrepareDirectLight(_streams, param);
    }
    o1S2C0_LightClusteredPointGroup_PrepareDirectLights(_streams);
    maxLightCount = o1S2C0_LightClusteredPointGroup_GetMaxLightCount(_streams);
    count = o1S2C0_LightClusteredPointGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param = i;
        o1S2C0_DirectLightGroup_PrepareDirectLight(_streams, param);
    }
    o2S2C0_DirectLightGroup_PrepareDirectLights();
    maxLightCount = o2S2C0_LightClusteredSpotGroup_GetMaxLightCount(_streams);
    count = o2S2C0_LightClusteredSpotGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param = i;
        o2S2C0_DirectLightGroup_PrepareDirectLight(_streams, param);
    }
    gl_Position = _streams.ShadingPosition_id12;
    VS_OUT_ScreenPosition = _streams.ScreenPosition_id13;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

