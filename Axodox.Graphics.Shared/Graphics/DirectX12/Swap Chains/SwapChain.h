#pragma once
#include "../Commands/CommandQueue.h"
#include "../Commands/CommandFence.h"

namespace Axodox::Graphics::DirectX12
{
  enum class SwapChainFlags
  {
    Default,
    IsShaderResource,
    IsTearingAllowed //For VRR (Variable Refresh Rate) support
  };

  class SwapChain/* : public GraphicsResource*/
  {
  public:
    void Resize();
    void Present();

    //RenderTarget2D* BackBuffer();

  protected:
    static const uint32_t _minBufferSize;
    Threading::manual_dispatcher _postPresentActions;

    SwapChain(const CommandQueue& queue, SwapChainFlags flags);

    DXGI_SWAP_CHAIN_DESC1 CreateDescription() const;

    virtual DirectX::XMUINT2 GetSize() const = 0;
    virtual DXGI_MATRIX_3X2_F GetTransformation() const = 0;

    void InitializeSwapChain(const winrt::com_ptr<IDXGISwapChain>& swapChain);

  private:
    //std::unordered_map<void*, std::unique_ptr<RenderTarget2D>> _buffers;
    winrt::com_ptr<IDXGISwapChain> _swapChain;
    SwapChainFlags _flags;
    CommandQueue _queue;
    CommandFence _fence;
    CommandFenceMarker _marker;
  };
}
