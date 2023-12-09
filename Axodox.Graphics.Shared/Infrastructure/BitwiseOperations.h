#pragma once
#include "pch.h"

namespace Axodox::Infrastructure
{
  template<typename T>
  std::span<const uint8_t> to_span(const T& value)
  {
    return { reinterpret_cast<const uint8_t*>(&value), sizeof(T) };
  }
}