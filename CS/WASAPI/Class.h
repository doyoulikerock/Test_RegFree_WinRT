#pragma once

#include "Class.g.h"

namespace winrt::WASAPI::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
        void Init();
    };
}

namespace winrt::WASAPI::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
