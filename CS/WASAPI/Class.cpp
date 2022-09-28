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

        //throw hresult_not_implemented();

        InitializeCapture();
    }


    void Class::ClearCapture()
    {
        if (m_deviceStateChangeToken)
        {
            m_capture->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
        }

        if (m_plotDataReadyToken)
        {
            m_capture->PlotDataReady(std::exchange(m_plotDataReadyToken, {}));
        }

        StopCapture();
        m_capture = nullptr;
    }



    //
   //  InitializeCapture()
   //
    void Class::InitializeCapture()
    {
        m_capture = nullptr;

        // Create a new WASAPI capture instance
        m_capture = make_self<WASAPICapture>();

        // Register for events
        m_deviceStateChangeToken = m_capture->DeviceStateChanged({ get_weak(), &Class::OnDeviceStateChange });
        //m_plotDataReadyToken = m_capture->PlotDataReady({ get_weak(), &Class::OnPlotDataReady });

        // There is an initial discontinuity when we start, so reset discontinuity counter
        // to -1 so that we ignore that initial discontinuity.
        m_discontinuityCount = -1;

        // Configure whether we are using low-latency capture
        m_isLowLatency = true; // toggleMinimumLatency().IsOn();
        m_capture->SetLowLatencyCapture(m_isLowLatency);

        // Perform the initialization
        m_capture->AsyncInitializeAudioDevice();
    }

    //
    //  StopCapture()
    //
    void Class::StopCapture()
    {
        if (m_capture)
        {
            // Set the event to stop playback
            m_capture->AsyncStopCapture();
        }
    }


#pragma region UI Related Code
    // Updates transport controls based on current playstate
    void Class::UpdateMediaControlUI(DeviceState deviceState)
    {
        switch (deviceState)
        {
        case DeviceState::Capturing:
            //btnStartCapture().IsEnabled(false);
            //btnStopCapture().IsEnabled(true);
            //toggleMinimumLatency().IsEnabled(false);
            break;

        case DeviceState::Stopped:
            //btnStartCapture().IsEnabled(true);
            //btnStopCapture().IsEnabled(false);
            //toggleMinimumLatency().IsEnabled(true);
            break;

        case DeviceState::Initialized:
        case DeviceState::Starting:
        case DeviceState::Stopping:
        case DeviceState::Flushing:
        case DeviceState::Error:
            /*btnStartCapture().IsEnabled(false);
            btnStopCapture().IsEnabled(false);*/
            break;
        }
    }
#pragma endregion




    // Event callback from WASAPI capture for changes in device state
    fire_and_forget Class::OnDeviceStateChange(IDeviceStateSource const&, WASAPI::DeviceStateChangedEventArgs e)
    {
        hresult result = 0;
        auto lifetime = get_strong();

        // Get the current time for messages
        std::time_t now = clock::to_time_t(clock::now());
        char buffer[26];
        ctime_s(buffer, ARRAYSIZE(buffer), &now);

        // Continue on UI thread.
        //co_await resume_foreground(Dispatcher());

        // Update Control Buttons
        DeviceState state = e.DeviceState();
        UpdateMediaControlUI(state);

        // Handle state specific messages
        switch (state)
        {
        case DeviceState::Initialized:
            m_capture->AsyncStartCapture();
            break;

        case DeviceState::Capturing:
            if (m_isLowLatency)
            {
                //rootPage.NotifyUser(L"Capture Started (minimum latency) at " + to_hstring(buffer), NotifyType::StatusMessage);
            }
            else
            {
                //rootPage.NotifyUser(L"Capture Started (normal latency) at " + to_hstring(buffer), NotifyType::StatusMessage);
            }
            break;

        case DeviceState::Discontinuity:
        {
            m_discontinuityCount++;

            if (m_discontinuityCount > 0)
            {
                //rootPage.NotifyUser(L"DISCONTINUITY DETECTED: " + to_hstring(buffer) + L" (Count = " + to_hstring(m_discontinuityCount) + L")", NotifyType::StatusMessage);
            }
        }
        break;

        case DeviceState::Flushing:
            if (m_plotDataReadyToken)
            {
                m_capture->PlotDataReady(std::exchange(m_plotDataReadyToken, {}));
            }

            //rootPage.NotifyUser(L"Finalizing WAV Header.  This may take a few minutes...", NotifyType::StatusMessage);

            //Oscilloscope().Points().ReplaceAll({ { OSC_START_X, OSC_START_Y }, { OSC_X_LENGTH, OSC_START_Y } });
            break;

        case DeviceState::Stopped:
            // For the stopped state, completely tear down the audio device
            ClearCapture();

            //rootPage.NotifyUser(L"Capture Stopped", NotifyType::StatusMessage);
            break;

        case DeviceState::Error:
            ClearCapture();

            // Specifically handle a couple of known errors
            switch (hresult error = e.ExtendedError(); error)
            {
            case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
                //rootPage.NotifyUser(L"ERROR: Check the Sound control panel for an active recording device.", NotifyType::ErrorMessage);
                break;

            case AUDCLNT_E_RESOURCES_INVALIDATED:
                //rootPage.NotifyUser(L"ERROR: Endpoint Lost Access To Resources", NotifyType::ErrorMessage);
                break;

            case E_ACCESSDENIED:
                //rootPage.NotifyUser(L"ERROR: Access Denied.  Check 'Settings->Permissions' for access to Microphone.", NotifyType::ErrorMessage);
                break;

            default:
                //rootPage.NotifyUser(L"ERROR: " + hresult_error(error).message(), NotifyType::ErrorMessage);
                break;
            }
        }

        //throw_hresult(result);
        return{};
    }

}
