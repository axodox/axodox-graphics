#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  enum class CommandKind
  {
    Direct = D3D12_COMMAND_LIST_TYPE_DIRECT,
    Compute = D3D12_COMMAND_LIST_TYPE_COMPUTE,
    Copy = D3D12_COMMAND_LIST_TYPE_COPY
  };
}