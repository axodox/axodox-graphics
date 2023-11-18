#pragma once

#include "Class.g.h"

namespace winrt::Axodox_Graphics_Universal::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::Axodox_Graphics_Universal::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
