#pragma once
#include "pch.h"

namespace Axodox::Infrastructure
{
  struct BufferSegment
  {
    uint64_t Start = 0, Size = 0;

    explicit operator bool() const noexcept;
  };

  uint64_t AlignMemoryOffset(uint64_t offset, uint64_t alignment);

  class BufferAllocator
  {
  public:
    BufferAllocator(uint64_t size);

    uint64_t Size() const;

    BufferSegment TryAllocate(uint64_t size, uint64_t alignment = 0);
    void Deallocate(BufferSegment segment);

  private:
    uint64_t _size;
    std::vector<BufferSegment> _freeSpace;
  };
}