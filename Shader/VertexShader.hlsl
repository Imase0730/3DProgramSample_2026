cbuffer Parameters : register(b0)
{
    float4x4 WorldViewProj;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    return mul(pos, WorldViewProj);
}
