#pragma once
#include "../GraphicsTypes.h"

namespace Axodox::Graphics::D3D12
{
  class DescriptorHeap;

  class Descriptor
  {
    friend struct DescriptorDeleter;

  public:
    Descriptor(DescriptorHeap* owner);

    Descriptor(const Descriptor&) = delete;
    Descriptor& operator=(const Descriptor&) = delete;

    virtual ~Descriptor() = default;

    void Realize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination);
    std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> Handle() const;
    explicit operator bool() const;

  protected:
    virtual void OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) = 0;
    
  private:
    DescriptorHeap* _owner;
    std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> _handle;
  };

  struct DescriptorDeleter
  {
    void operator()(Descriptor* descriptor);
  };

  template<typename T>
  using descriptor_ptr = std::unique_ptr<T, DescriptorDeleter>;
}