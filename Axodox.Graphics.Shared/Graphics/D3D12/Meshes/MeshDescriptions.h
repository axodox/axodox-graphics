#pragma once
#include "../Enumerations.h"
#include "../Resources/BufferDefinition.h"

namespace Axodox::Graphics::D3D12
{
  struct AXODOX_GRAPHICS_API MeshDescription
  {
    PrimitiveTopology Topology = PrimitiveTopology::Undefined;
    BufferData Vertices;
    BufferData Indices;
  };
}