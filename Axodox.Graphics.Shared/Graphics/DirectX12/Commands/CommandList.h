#pragma once
#include "CommandKind.h"

namespace Axodox::Graphics::D3D12
{
  class CommandList
  {
    friend class CommandAllocator;
    friend class CommandQueue;

  private:
    winrt::com_ptr<ID3D12GraphicsCommandList7> _list;
  };
}