cbuffer Constants : register(b0)
{
  float4x4 Transformation;
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
  return mul(float4(pos, 1), Transformation);
}