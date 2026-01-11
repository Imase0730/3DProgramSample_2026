#include "Header.hlsli"

float3 ComputeLight(float3 worldNormal)
{
    float3 dotL = mul(-LightDirection, worldNormal);
    float3 zeroL = step(0, dotL);
    return zeroL * dotL;
}

VSOutput main(VSInput vin)
{
    VSOutput vout;
    
    vout.Position = mul(vin.Position, WorldViewProj);
    vout.Diffuse = float4(ComputeLight(vin.Normal), 1.0f);
    vout.TexCoord = vin.TexCoord;
    
    return vout;
}

