cbuffer Parameters : register(b0)
{
    float4x4 WorldViewProj;
};

struct VSInput
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
};

struct VSOutput
{
    float4 Color : COLOR0;
    float4 Position : SV_Position;
};
