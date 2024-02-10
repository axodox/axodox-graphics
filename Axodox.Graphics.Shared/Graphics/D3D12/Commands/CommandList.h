#pragma once
#include "CommandKind.h"
#include "CommandFenceMarker.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API CommandList
  {
    friend class CommandAllocator;
    friend class CommandQueue;

  public:
    CommandList() = default;

    CommandList(const CommandList&) = delete;
    CommandList& operator =(const CommandList&) = delete;

    CommandList(CommandList&&) = default;
    CommandList& operator =(CommandList&&) = default;

  private:
    winrt::com_ptr<ID3D12GraphicsCommandList7> _list;
    std::vector<CommandFenceMarker> _signalers, _awaiters;
  };
}