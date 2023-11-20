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

  class SwapChain
  {
  public:
    void Resize();
    void Present();

    RenderTargetView RenderTargetView();

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
    std::vector<D3D12::RenderTargetView> _targets;
    RenderTargetDescriptorHeap _rtvHeap;
    SwapChainFlags _flags;
    CommandQueue _queue;
    CommandFence _fence;
    CommandFenceMarker _marker;
  };
}
