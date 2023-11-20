#pragma once
#include "pch.h"
#include "CommandQueue.h"
#include "Collections/ObjectPool.h"

namespace Axodox::Graphics::D3D12
{
  class GraphicsDevice;

  using CommandFenceTimeout = std::chrono::duration<uint32_t, std::milli>;

  class CommandFence;
  class CommandFenceMarker
  {
  public:
    CommandFenceMarker();

    explicit operator bool() const;

  private:
    friend class CommandFence;

    CommandFenceMarker(CommandFence* owner, uint64_t value);

    CommandFence* Owner;
    uint64_t Value;
  };  

  class CommandFence
  {
  public:
    CommandFence(const GraphicsDevice& device);

    CommandFence(const CommandFence&) = delete;
    CommandFence& operator=(const CommandFence&) = delete;

    CommandFence(CommandFence&&) = delete;
    CommandFence& operator=(CommandFence&&) = delete;

    [[nodiscard]] CommandFenceMarker CreateMarker();

    [[nodiscard]] CommandFenceMarker EnqueueSignal(const CommandQueue& queue);
    void EnqueueSignal(const CommandQueue& queue, CommandFenceMarker marker) const;

    [[nodiscard]] CommandFenceMarker EnqueueAwait(const CommandQueue& queue);
    void EnqueueAwait(const CommandQueue& queue, CommandFenceMarker marker) const;

    void Signal(CommandFenceMarker marker);
    bool Await(CommandFenceMarker marker, CommandFenceTimeout timeout = CommandFenceTimeout(INFINITE));

  private:
    winrt::com_ptr<ID3D12Fence> _fence;
    uint64_t _value;

    void CheckMarker(CommandFenceMarker marker) const;
  };
}