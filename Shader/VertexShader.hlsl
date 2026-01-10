#include "Header.hlsli"

VSOutput main(VSInput vin)
{
    VSOutput vout;
    
    vout.Position = mul(vin.Position, WorldViewProj);
    vout.Color = vin.Color;
    
    return vout;
}

