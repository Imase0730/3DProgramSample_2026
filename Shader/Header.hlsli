cbuffer Parameters : register(b0)
{
    float4x4 WorldViewProj;
    float3 LightDirection;
};

struct VSInput
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct VSOutput
{
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};
