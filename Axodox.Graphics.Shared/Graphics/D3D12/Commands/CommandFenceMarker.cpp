#include "pch.h"
#include "CommandFenceMarker.h"
#include "CommandFence.h"
#include "CommandQueue.h"

namespace Axodox::Graphics::D3D12
{
  CommandFenceMarker::CommandFenceMarker() :
    Owner(nullptr),
    Value(0)
  { }

  void CommandFenceMarker::Signal(const CommandQueue& queue)
  {
    if (Owner) queue->Signal(Owner->get(), Value);
  }

  void CommandFenceMarker::Await(const CommandQueue& queue)
  {
    if (Owner) queue->Wait(Owner->get(), Value);
  }

  CommandFenceMarker::CommandFenceMarker(CommandFence* owner, uint64_t value) :
    Owner(owner),
    Value(value)
  { }
}