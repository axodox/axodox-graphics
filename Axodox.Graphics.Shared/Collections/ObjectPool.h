#pragma once
#include "pch.h"

namespace Axodox::Collections
{
  template<typename T>
  class ObjectPool;

  template<typename T>
  class ObjectPoolHandle
  {
    friend class ObjectPool<T>;

  public:
    T& operator*() noexcept
    {
      return _object;
    }

    T* operator->() noexcept
    {
      return &_object;
    }

    ~ObjectPoolHandle();

  private:
    ObjectPoolHandle(ObjectPool<T>* owner, T&& value) :
      _owner(owner),
      _object(std::move(value))
    {}

    ObjectPool<T>* _owner;
    T _object;
  };

  template<typename T>
  class ObjectPool
  {
    friend class ObjectPoolHandle<T>;

  public:
    ObjectPool(const std::function<T()>& factory = [] { return T{}; }) :
      _factory(factory)
    { }

    [[nodiscard]] ObjectPoolHandle<T> Borrow()
    {
      {
        std::lock_guard lock(_mutex);
        if (_objects.empty())
        {
          return { this, _factory()};
        }
      }

      {
        T object{ std::move(_objects.front()) };
        _objects.pop();
        return { this, std::move(object) };
      }
    }

  private:
    std::mutex _mutex;
    std::queue<T> _objects;
    std::function<T()> _factory;

    void Return(T&& value)
    {
      std::lock_guard lock(_mutex);
      _objects.emplace(std::move(value));
    }
  };

  template<typename T>
  ObjectPoolHandle<T>::~ObjectPoolHandle()
  {
    _owner->Return(std::move(_object));
  }
}