#include "pch.h"
#include "TextureDefinition.h"

namespace Axodox::Graphics::D3D12
{
  TextureDefinition::operator D3D12_RESOURCE_DESC() const
  {
    auto dimension = 1u;
    for (auto size : { Width, Height, Depth })
    {
      if (size > 0) dimension++; else break;
    }
    
    return D3D12_RESOURCE_DESC{
      .Dimension = D3D12_RESOURCE_DIMENSION(dimension),
      .Alignment = 0ull,
      .Width = Width,
      .Height = dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D ? 1u : Height,
      .DepthOrArraySize = dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? uint16_t(Depth) : ArraySize,
      .MipLevels = MipLevels,
      .Format = DXGI_FORMAT(PixelFormat),
      .SampleDesc = { SampleCount, SampleQuality },
      .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
      .Flags = D3D12_RESOURCE_FLAGS(Flags)
    };
  }

  BufferDefinition::operator D3D12_RESOURCE_DESC() const
  {
    return D3D12_RESOURCE_DESC{
      .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
      .Alignment = 0ull,
      .Width = Length,
      .Height = 1u,
      .DepthOrArraySize = 1u,
      .MipLevels = 1u,
      .Format = DXGI_FORMAT_UNKNOWN,
      .SampleDesc = { 1u, 0u },
      .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
      .Flags = D3D12_RESOURCE_FLAGS(Flags)
    };
  }
}