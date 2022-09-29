#pragma once
#include "pch.h"
#include "Env.g.h"

namespace winrt::WASAPI::implementation
{
   

    struct Env : EnvT<Env>
    {
        Env() = default;

        int32_t Prm1();
        void Prm1(int32_t value);

    private:
        int32_t prm1;
    };
}

namespace winrt::WASAPI::factory_implementation
{
    struct Env : EnvT<Env, implementation::Env>
    {
    };
    
}
