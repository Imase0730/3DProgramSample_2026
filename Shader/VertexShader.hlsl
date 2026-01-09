cbuffer Parameters : register(b0)
{
    float4x4 WorldViewProj;
};

struct VSInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VSOutput
{
    float4 Color : COLOR;
    float4 Position : SV_POSITION;  // ç≈å„Ç…èëÇ≠Ç±Ç∆
};

VSOutput main(VSInput vin)
{
    VSOutput vout;
    
    vout.Position = mul(float4(vin.Position, 1.0f), WorldViewProj);
    vout.Color = vin.Color;
    
    return vout;
}

