#pragma once
#include "Descriptor.h"
#include "../Devices/GraphicsDevice.h"

namespace Axodox::Graphics::D3D12
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
    friend struct DescriptorDeleter;

  public:
    DescriptorHeap(const GraphicsDevice& device, DescriptorHeapKind type);
    
    DescriptorHeap(const DescriptorHeap&) = delete;
    DescriptorHeap& operator=(const DescriptorHeap&) = delete;
    
    virtual ~DescriptorHeap();    

    DescriptorHeapKind Type() const;

    void Build();
    void Clean();

  protected:
    template<typename T, typename... TArgs>
    descriptor_ptr<T> CreateDescriptor(TArgs&&... args)
    {
      auto descriptor = std::make_unique<T>(this, std::forward<TArgs>(args)...);
      auto handle = descriptor_ptr<T>(descriptor.get());

      std::lock_guard lock(_mutex);
      _items.push_back(move(descriptor));
      _isDirty = true;
      return handle;
    }

  private:
    GraphicsDevice _device;
    DescriptorHeapKind _type;
    std::mutex _mutex;
    winrt::com_ptr<ID3D12DescriptorHeap> _heap;
    std::vector<std::unique_ptr<Descriptor>> _items;
    std::set<const Descriptor*> _reclaimables;
    bool _isDirty = false;

    void DeleteDescriptor(const Descriptor* descriptor);
  };
}