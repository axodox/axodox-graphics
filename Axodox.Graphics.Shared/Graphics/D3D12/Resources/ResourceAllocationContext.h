#pragma once
#include "ResourceAllocator.h"
#include "ResourceUploader.h"
#include "../Descriptors/CommonDescriptorHeap.h"
#include "../Descriptors/RenderTargetView.h"
#include "../Descriptors/DepthStencilView.h"

namespace Axodox::Graphics::D3D12
{
  struct ResourceAllocationContext
  {
    GraphicsDevice* Device = nullptr;
    ResourceAllocator* ResourceAllocator = nullptr;
    ResourceUploader* ResourceUploader = nullptr;
    CommonDescriptorHeap* CommonDescriptorHeap = nullptr;
    RenderTargetDescriptorHeap* RenderTargetDescriptorHeap = nullptr;
    DepthStencilDescriptorHeap* DepthStencilDescriptorHeap = nullptr;
  };
}