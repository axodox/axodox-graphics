#pragma once
#include "ResourceAllocator.h"
#include "ResourceUploader.h"
#include "../Descriptors/CommonDescriptorHeap.h"

namespace Axodox::Graphics::D3D12
{
  struct ResourceAllocationContext
  {
    ResourceAllocator* ResourceAllocator;
    ResourceUploader* ResourceUploader;
    CommonDescriptorHeap* DescriptorHeap;
  };
}