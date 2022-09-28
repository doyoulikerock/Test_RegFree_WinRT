#include "pch.h"
#include "Class.h"
#include "Class.g.cpp"

namespace winrt::WASAPI::implementation
{
    int32_t Class::MyProperty()
    {
        return 1000;
    }

    void Class::MyProperty(int32_t value)
    {
        printf("%d\n", value);
    }

    void Class::Init()
    {
        throw hresult_not_implemented();
    }
}
