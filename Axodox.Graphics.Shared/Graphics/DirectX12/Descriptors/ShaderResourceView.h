#pragma once
#include "SimpleResourceView.h"

namespace Axodox::Graphics::D3D12
{
  using ShaderResourceView = SimpleResourceView<D3D12_SHADER_RESOURCE_VIEW_DESC, &ID3D12Device::CreateShaderResourceView>;
}