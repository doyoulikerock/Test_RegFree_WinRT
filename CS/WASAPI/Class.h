#pragma once
#include "pch.h"
#include "Class.g.h"
#include "WASAPICapture.h"


namespace winrt::WASAPI::implementation
{
    /// <summary>
    /// base class of WASAPI
    /// </summary>
    struct Class : ClassT<Class>, MediaFoundationInitializer
    {
        Class() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);

        
        void Init();
    private:
        event_token m_deviceStateChangeToken;
        event_token m_plotDataReadyToken;

        int m_discontinuityCount;
        bool m_isLowLatency;
        com_ptr<WASAPICapture> m_capture;

        // UI Helpers
        void UpdateMediaControlUI(DeviceState deviceState);

        // Handlers
        fire_and_forget OnDeviceStateChange(IDeviceStateSource const& sender, WASAPI::DeviceStateChangedEventArgs e);
        //fire_and_forget OnPlotDataReady(IPlotDataSource const& sender, WASAPI::PlotDataReadyEventArgs e);*/

        void InitializeCapture();
        void StopCapture();
        void ClearCapture();
    };
}

namespace winrt::WASAPI::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
