#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  typedef std::span<const D3D12_INPUT_ELEMENT_DESC> VertexDefinition;

  struct VertexPosition
  {
    DirectX::XMFLOAT3 Position;

    static const VertexDefinition Layout;
  };

  struct VertexPositionColor
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMUBYTEN4 Color;

    static const VertexDefinition Layout;
  };

  struct VertexPositionTexture
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMUSHORTN2 Texture;

    static const VertexDefinition Layout;
  };

  struct VertexPositionNormal
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMBYTEN4 Normal;

    static const VertexDefinition Layout;
  };

  struct VertexPositionNormalColor
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMBYTEN4 Normal;
    DirectX::PackedVector::XMUBYTEN4 Color;

    static const VertexDefinition Layout;
  };

  struct VertexPositionNormalTexture
  {
    DirectX::XMFLOAT3 Position;
    DirectX::PackedVector::XMBYTEN4 Normal;
    DirectX::PackedVector::XMUSHORTN2 Texture;

    static const VertexDefinition Layout;
  };
}
