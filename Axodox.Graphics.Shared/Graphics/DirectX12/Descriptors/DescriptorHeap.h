#pragma once
#include "Descriptor.h"
#include "../Devices/GraphicsDevice.h"

namespace Axodox::Graphics::DirectX12
{
  enum class DescriptorHeapKind
  {
    ShaderResource = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
    SamplerState = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
    RenderTarget = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
    DepthStencil = D3D12_DESCRIPTOR_HEAP_TYPE_DSV
  };

  class DescriptorHeap
  {
  public:
    DescriptorHeap(const GraphicsDevice& device, DescriptorHeapKind type);
    
    DescriptorHeap(const DescriptorHeap&) = delete;
    DescriptorHeap& operator=(const DescriptorHeap&) = delete;
    
    virtual ~DescriptorHeap() = default;    

    DescriptorHeapKind Type() const;

    void Build();

  protected:
    void AddDescriptor(const std::shared_ptr<Descriptor>& descriptor);

    template<typename T, typename... TArgs>
    T CreateDescriptor(TArgs&&... args)
    {
      auto descriptor = std::make_shared<typename T::descriptor_t>(std::forward<TArgs>(args)...);
      AddDescriptor(descriptor);
      return T(std::move(descriptor));
    }

  private:
    GraphicsDevice _device;
    DescriptorHeapKind _type;
    winrt::com_ptr<ID3D12DescriptorHeap> _heap;
    std::vector<std::weak_ptr<Descriptor>> _items;
    bool _isDirty = false;

    std::vector<std::shared_ptr<Descriptor>> PinAndClean();
  };
}