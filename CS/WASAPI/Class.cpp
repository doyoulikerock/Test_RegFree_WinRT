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


    /// <summary>
    /// initialize capture/renderer
    /// </summary>
    void Class::Init()
    {
        //unique_shared_work_queue m_queueId{ L"Capture" };

        throw hresult_not_implemented();
    }
}
