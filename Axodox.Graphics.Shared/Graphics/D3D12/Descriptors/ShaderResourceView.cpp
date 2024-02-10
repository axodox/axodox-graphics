#include "pch.h"
#include "ShaderResourceView.h"
#include "CommonDescriptorHeap.h"
#include "../Resources/BufferDefinition.h"
#include "../Resources/TextureDefinition.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  const ResourceDefinition* ShaderResourceView::Definition() const
  {
    return _definition.get();
  }

  D3D12_GPU_DESCRIPTOR_HANDLE ShaderResourceView::GpuHandle() const
  {
    return static_cast<CommonDescriptorHeap*>(_owner)->ResolveGpuHandle(_handle);
  }

  ShaderResourceView::operator GpuVirtualAddress() const
  {
    return GpuHandle().ptr;
  }

  void ShaderResourceView::OnInit()
  {
    auto description = _resource->GetDesc();
    if (description.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
    {
      _definition = make_unique<BufferDefinition>(description);
    }
    else
    {
      _definition = make_unique<TextureDefinition>(description);
    }
  }
}