#include "pch.h"
#include "CommittedResourceAllocator.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  void CommittedResourceAllocator::AllocateResources(ResourceSpan resources)
  {
    //Create all resources which do not exist
    for (auto& resource : resources)
    {
      if (!resource->get()) resource->AllocateCommitted(_device);
    }
  }
}