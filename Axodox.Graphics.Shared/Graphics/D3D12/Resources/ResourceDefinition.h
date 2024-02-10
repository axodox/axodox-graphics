#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  enum class ResourceType
  {
    Unknown,
    Buffer,
    Texture
  };

  struct AXODOX_GRAPHICS_API ResourceDefinition
  {
    virtual ResourceType Type() const = 0;
    virtual explicit operator D3D12_RESOURCE_DESC() const = 0;
    virtual ~ResourceDefinition() = default;
  };
}