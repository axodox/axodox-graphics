#pragma once
#include "../GraphicsTypes.h"

namespace Axodox::Graphics::D3D12
{
  class DescriptorHeap;

  class AXODOX_GRAPHICS_API Descriptor
  {
    friend struct DescriptorDeleter;

  public:
    Descriptor(DescriptorHeap* owner);

    Descriptor(const Descriptor&) = delete;
    Descriptor& operator=(const Descriptor&) = delete;

    virtual ~Descriptor() = default;

    void Realize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination);
    D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle() const;
    explicit operator bool() const;

  protected:
    DescriptorHeap* _owner;
    D3D12_CPU_DESCRIPTOR_HANDLE _handle;

    virtual void OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) = 0;
  };

  struct AXODOX_GRAPHICS_API DescriptorDeleter
  {
    void operator()(Descriptor* descriptor);
  };

  template<typename T>
  using descriptor_ptr = std::unique_ptr<T, DescriptorDeleter>;
}