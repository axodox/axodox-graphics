#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  typedef std::span<const D3D12_INPUT_ELEMENT_DESC> VertexDefinition;

  struct AXODOX_GRAPHICS_API VertexPosition
  {
    DirectX::XMFLOAT3 Position;

    static const VertexDefinition Layout;
  };

  struct AXODOX_GRAPHICS_API VertexPositionColor
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMUBYTEN4 Color;

    static const VertexDefinition Layout;
  };

  struct AXODOX_GRAPHICS_API VertexPositionTexture
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMUSHORTN2 Texture;

    static const VertexDefinition Layout;
  };

  struct AXODOX_GRAPHICS_API VertexPositionNormal
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMBYTEN4 Normal;

    static const VertexDefinition Layout;
  };

  struct AXODOX_GRAPHICS_API VertexPositionNormalColor
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMBYTEN4 Normal;
    DirectX::PackedVector::XMUBYTEN4 Color;

    static const VertexDefinition Layout;
  };

  struct AXODOX_GRAPHICS_API VertexPositionNormalTexture
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMBYTEN4 Normal;
    DirectX::PackedVector::XMUSHORTN2 Texture;

    static const VertexDefinition Layout;
  };
}
