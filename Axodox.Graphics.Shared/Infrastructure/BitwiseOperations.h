#pragma once
#include "pch.h"

namespace Axodox::Infrastructure
{
  template<typename T>
  std::span<const uint8_t> to_span(const T& value)
  {
    return { reinterpret_cast<const uint8_t*>(&value), sizeof(T) };
  }

  template<typename T>
  std::span<const uint8_t> to_span(std::span<const T> span)
  {
    return { reinterpret_cast<const uint8_t*>(span.data()), sizeof(T) * span.size() };
  }

  template<typename T>
  std::span<const uint8_t> to_span(std::initializer_list<T> list)
  {
    return { reinterpret_cast<const uint8_t*>(list.begin()), reinterpret_cast<const uint8_t*>(list.end()) };
  }

  template<typename T>
  std::vector<uint8_t> to_vector(std::span<const T> span)
  {
    return { reinterpret_cast<const uint8_t*>(span.data()), sizeof(T) * span.size() };
  }

  template<typename T>
  std::vector<uint8_t> to_vector(std::initializer_list<T> list)
  {
    return { reinterpret_cast<const uint8_t*>(list.begin()), reinterpret_cast<const uint8_t*>(list.end()) };
  }

}