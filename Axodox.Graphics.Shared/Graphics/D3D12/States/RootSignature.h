#pragma once
#include "../Devices/GraphicsDevice.h"
#include "RootSignatureMask.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API RootSignatureBase
  {
  public:
    RootSignatureBase(const GraphicsDevice& device);

    ID3D12RootSignature* get() const;

  protected:
    RootSignatureBlueprint _blueprint;
    winrt::com_ptr<ID3D12RootSignature> _signature;

    winrt::com_ptr<ID3D12RootSignature> BuildSignature(const RootSignatureMask* signature) const;
    void SetSignature(RootSignatureMask* mask, CommandAllocator& allocator, RootSignatureUsage usage) const;

  private:
    GraphicsDevice _device;
  };

  template<typename T>
  class RootSignature : public RootSignatureBase
  {
  public:
    RootSignature(const GraphicsDevice& device) :
      RootSignatureBase(device),
      _mask(RootSignatureContext{ &_blueprint })
    {
      _signature = BuildSignature(&_mask);
    }

    [[nodiscard]] T Set(CommandAllocator& allocator, RootSignatureUsage usage) const
    {
      T result = _mask;
      SetSignature(&result, allocator, usage);
      return result;
    }

  private:
    T _mask;
  };
}