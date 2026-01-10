#include "Header.hlsli"

float4 main(VSOutput pin) : SV_TARGET
{
    return pin.Color;
}
