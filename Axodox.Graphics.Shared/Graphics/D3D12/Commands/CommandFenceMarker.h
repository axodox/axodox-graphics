#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  class CommandFence;
  class CommandQueue;

  class AXODOX_GRAPHICS_API CommandFenceMarker
  {
  public:
    CommandFenceMarker();

    void Signal(const CommandQueue& queue);
    void Await(const CommandQueue& queue);

    explicit operator bool() const;

  private:
    friend class CommandFence;

    CommandFenceMarker(CommandFence* owner, uint64_t value);

    CommandFence* Owner;
    uint64_t Value;
  };
}