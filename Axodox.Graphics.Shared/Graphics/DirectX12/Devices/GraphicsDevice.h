#pragma once
#include "../GraphicsTypes.h"
#include "../Commands/CommandQueue.h"

namespace Axodox::Graphics::DirectX12
{
  class GraphicsDevice
  {
  public:
    explicit GraphicsDevice(ID3D12DeviceChild* deviceChild);
    explicit GraphicsDevice(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0);

    ID3D12DeviceT* get() const;
    ID3D12DeviceT* operator->() const;

    CommandQueue CreateQueue(CommandQueueKind type = CommandQueueKind::Direct) const;

  private:
    winrt::com_ptr<ID3D12DeviceT> _device;
  };
}