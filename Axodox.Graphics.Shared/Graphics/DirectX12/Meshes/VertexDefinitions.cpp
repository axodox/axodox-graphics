#include "pch.h"
#include "VertexDefinitions.h"

namespace
{
  consteval D3D12_INPUT_ELEMENT_DESC InputElement(const char* semanticName, uint32_t semanticIndex, DXGI_FORMAT format)
  {
    return {
      .SemanticName = semanticName,
      .SemanticIndex = semanticIndex,
      .Format = format,
      .InputSlot = 0,
      .AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
      .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
      .InstanceDataStepRate = 0
    };
  }

  const D3D12_INPUT_ELEMENT_DESC VertexPositionDefinition[] = {
    InputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT)
  };

  const D3D12_INPUT_ELEMENT_DESC VertexPositionColorDefinition[] = {
    InputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT),
    InputElement("COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM)
  };

  const D3D12_INPUT_ELEMENT_DESC VertexPositionTextureDefinition[] = {
    InputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT),
    InputElement("TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM)
  };

  const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalDefinition[] = {
    InputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT),
    InputElement("NORMAL", 0, DXGI_FORMAT_R8G8B8A8_SNORM)
  };

  const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalColorDefinition[] = {
    InputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT),
    InputElement("NORMAL", 0, DXGI_FORMAT_R8G8B8A8_SNORM),
    InputElement("COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM)
  };

  const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTextureDefinition[] = {
    InputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT),
    InputElement("NORMAL", 0, DXGI_FORMAT_R8G8B8A8_SNORM),
    InputElement("TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM)
  };
}

namespace Axodox::Graphics::D3D12
{
  const VertexDefinition VertexPosition::Layout = VertexPositionDefinition;
  const VertexDefinition VertexPositionColor::Layout = VertexPositionColorDefinition;
  const VertexDefinition VertexPositionTexture::Layout = VertexPositionTextureDefinition;
  const VertexDefinition VertexPositionNormal::Layout = VertexPositionNormalDefinition;
  const VertexDefinition VertexPositionNormalColor::Layout = VertexPositionNormalColorDefinition;
  const VertexDefinition VertexPositionNormalTexture::Layout = VertexPositionNormalTextureDefinition;
}