Texture2D _texture : register(t0);
SamplerState _sampler : register(s0);

struct input_t
{
  float4 Screen : SV_POSITION;
  float2 Texture : TEXCOORD;
};

float4 main(input_t input) : SV_TARGET
{
  return _texture.Sample(_sampler, input.Texture);
}