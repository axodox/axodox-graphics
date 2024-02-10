#pragma once
#include "../Commands/CommandQueue.h"
#include "../Commands/CommandFence.h"
#include "../Descriptors/RenderTargetView.h"

namespace Axodox::Graphics::D3D12
{
  enum class SwapChainFlags
  {
    Default,
    IsShaderResource,
    IsTearingAllowed //For VRR (Variable Refresh Rate) support
  };

  class AXODOX_GRAPHICS_API SwapChain
  {
    Infrastructure::event_owner _events;

  public:
    void Resize();
    void Present();

    DirectX::XMUINT2 Resolution() const;

    const RenderTargetView* RenderTargetView();

    Infrastructure::event_publisher<SwapChain*> Resizing;

  protected:
    static const uint32_t _minBufferSize;
    Threading::manual_dispatcher _postPresentActions;

    SwapChain(const CommandQueue& queue, SwapChainFlags flags);

    DXGI_SWAP_CHAIN_DESC1 CreateDescription() const;

    virtual DirectX::XMUINT2 GetSize() const = 0;
    virtual DXGI_MATRIX_3X2_F GetTransformation() const = 0;

    void InitializeSwapChain(const winrt::com_ptr<IDXGISwapChain>& swapChain);

  private:
    winrt::com_ptr<IDXGISwapChain3> _swapChain;
    std::vector<D3D12::RenderTargetViewRef> _targets;
    RenderTargetDescriptorHeap _rtvHeap;
    SwapChainFlags _flags;
    CommandQueue _queue;
    CommandFence _fence;
    CommandFenceMarker _marker;
    DirectX::XMUINT2 _resolution;
  };
}
