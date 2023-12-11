#pragma once
#include "DescriptorHeap.h"
#include "ShaderResourceView.h"
#include "../Resources/Resource.h"

namespace Axodox::Graphics::D3D12
{
  class CommonDescriptorHeap : public DescriptorHeap
  {
  public:
    CommonDescriptorHeap(const GraphicsDevice& device, uint32_t framesInFlight);

    descriptor_ptr<ShaderResourceView> CreateShaderResourceView(Resource* resource);

    D3D12_GPU_DESCRIPTOR_HANDLE ResolveGpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);

    void Set(CommandAllocator& allocator);

  protected:
    virtual void OnHeapBuilt(ID3D12DescriptorHeap* heap, uint32_t descriptorCount) override;

  private:
    std::vector<winrt::com_ptr<ID3D12DescriptorHeap>> _heaps;
    D3D12_GPU_DESCRIPTOR_HANDLE _handleBase;
    uint32_t _frameIndex;
  };
}