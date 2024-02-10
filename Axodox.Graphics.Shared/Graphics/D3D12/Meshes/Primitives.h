#pragma once
#include "VertexDefinitions.h"
#include "MeshDescriptions.h"

namespace Axodox::Graphics::D3D12
{
  AXODOX_GRAPHICS_API MeshDescription CreateQuad(float size = 1.f);
  AXODOX_GRAPHICS_API MeshDescription CreateCube(float size = 1.f, bool useSameTextureOnAllFaces = true);
  AXODOX_GRAPHICS_API MeshDescription CreatePlane(float size, DirectX::XMUINT2 subdivisions);
}