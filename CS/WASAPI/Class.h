#pragma once
#include "pch.h"
#include "Class.g.h"
#include "WASAPICapture.h"
#include "WASAPIRenderer.h"

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

        
        void Init(int32_t value);
    private:
        /** **************************************  capture */
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


        /** ************************************  renderer */

        Windows::Media::SystemMediaTransportControls m_systemMediaControls{ nullptr };
        event_token m_RenderdeviceStateChangeToken;
        event_token m_systemMediaControlsButtonToken;

        bool m_deviceSupportsRawMode = false;
        bool m_isMinimumLatency = false;

        Windows::Storage::Streams::IRandomAccessStream m_contentStream{ nullptr };
        ContentType m_contentType = ContentType::Tone;
        com_ptr<WASAPIRenderer> m_renderer;

        // page is selected:
        fire_and_forget OnNavigatedTo();
        //void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        // leave this page
        void OnNavigatedFrom();
        fire_and_forget MediaButtonPressed(Windows::Media::SystemMediaTransportControls const& sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs e);


        // UI helpers
        void UpdateRenderControlUI(DeviceState deviceState);

        // event Handlers
        fire_and_forget OnRenderDeviceStateChange(IDeviceStateSource const& sender, WASAPI::DeviceStateChangedEventArgs e);

        void InitializeDevice();
        void StartDevice();
        void StopDevice();
        void PauseDevice();
        void PlayPauseToggleDevice();

        void OnSetVolume(double volume);

    };
}

namespace winrt::WASAPI::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
