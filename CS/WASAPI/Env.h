#pragma once
#include "pch.h"
#include "Env.g.h"

namespace winrt::WASAPI::implementation
{
   

    struct Env : EnvT<Env>
    {
        Env() = default;

        /// <summary>
        /// test of prm1
        /// </summary>        
        int32_t Prm1();
        void Prm1(int32_t value);

        /// <summary>
        /// test of numprm
        /// </summary>
        /// <returns></returns>
        static int32_t NumPrms();
        static void NumPrms(int32_t v);
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
