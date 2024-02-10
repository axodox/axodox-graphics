#pragma once
#include "VertexDefinitions.h"
#include "MeshDescriptions.h"

namespace Axodox::Graphics::D3D12
{
  MeshDescription CreateQuad(float size = 1.f);
  MeshDescription CreateCube(float size = 1.f, bool useSameTextureOnAllFaces = true);
  MeshDescription CreatePlane(float size, DirectX::XMUINT2 subdivisions);
}