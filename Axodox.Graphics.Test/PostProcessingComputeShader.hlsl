Texture2D _input : register(t0);
RWTexture2D<unorm float4> _output : register(u0);
SamplerState _sampler : register(s0);

cbuffer Constants : register(b0)
{
  float Time;
}

[numthreads(16, 16, 1)]
void main(uint3 index : SV_DispatchThreadID )
{
  uint2 size;
  _input.GetDimensions(size.x, size.y);
  
  float2 uv = (index.xy + 0.5) / size;
  uv.x += 0.01 * sin(uv.y * 20 + Time);
  float4 color = _input.SampleLevel(_sampler, uv, 0);
  
  _output[index.xy] = color;
}