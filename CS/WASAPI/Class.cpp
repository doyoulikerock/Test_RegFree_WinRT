#include "pch.h"
#include "Class.h"
#include "Class.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::Storage::Streams;


//const GUID MF_MT_MAJOR_TYPE = { 0x48eba18e, 0xf8c9, 0x4687, 0xbf, 0x11, 0x0a, 0x74, 0xc9, 0xf9, 0x6a, 0x8f };
//const GUID MFMediaType_Audio = { 0x73647561, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };
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
    void Class::Init(int32_t value)
    {
        //unique_shared_work_queue m_queueId{ L"Capture" };

        //throw hresult_not_implemented();



        //com_ptr<IMFMediaType> mediaType;
        //// Create a partial media type for our mix format (PCM or IEEE Float)
        //check_hresult(MFCreateMediaType(mediaType.put()));
        //// Set media type attributes
        //check_hresult(mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));



        if(value ==0)
            InitializeCapture();
        else {
            // init page
            OnNavigatedTo();
            StartDevice();
        }
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



#pragma region Renderer

    //
    //  OnSetVolume()
    //
    //  Updates the session volume
    //
    void Class::OnSetVolume(double volume)
    {
        if (m_renderer)
        {
            // Updates the Session volume on the AudioClient
            m_renderer->SetVolumeOnSession(volume);
        }
    }

    //
   //  InitializeDevice()
   //
   //  Sets up a new instance of the WASAPI renderer
   //
    void Class::InitializeDevice()
    {
        if (m_renderer)
        {
            // Already initialized.
            return;
        }

        // Create a new WASAPI instance
        m_renderer = make_self<WASAPIRenderer>();

        // Register for events
        m_deviceStateChangeToken = m_renderer->DeviceStateChanged({ get_weak(), &Class::OnRenderDeviceStateChange });

        // Configure user based properties
        m_renderer->SetContentType(m_contentType);
        m_renderer->SetFrequency(480); // sliderFrequency().Value());
        m_renderer->SetContentStream(m_contentStream);

        m_isMinimumLatency = true; // toggleMinimumLatency().IsOn();
        m_renderer->SetLowLatency(m_isMinimumLatency);
        m_renderer->SetHWOffload(false);
        m_renderer->SetBackgroundAudio(false);
        m_renderer->SetRawAudio(m_deviceSupportsRawMode && true); // toggleRawAudio().IsOn());

        // Selects the Default Audio Device
        m_renderer->AsyncInitializeAudioDevice();

        // Set the initial volume.
        OnSetVolume(10); // sliderVolume().Value());
    }

    void Class::StartDevice()
    {
        if (m_renderer == nullptr)
        {
            // Call from main UI thread
            InitializeDevice();
        }
        else
        {
            // Starts a work item to begin playback, likely in the paused state
            m_renderer->StartPlayback();
        }
    }

    void Class::StopDevice()
    {
        if (m_renderer)
        {
            // Set the event to stop playback
            m_renderer->StopPlaybackAsync();
        }
    }

    void Class::PauseDevice()
    {
        if (m_renderer)
        {
            if (m_renderer->DeviceState() == DeviceState::Playing)
            {
                // Starts a work item to pause playback
                m_renderer->PausePlaybackAsync();
            }
        }
    }

    //
    //  PlayPauseToggleDevice()
    //
    void Class::PlayPauseToggleDevice()
    {
        if (m_renderer)
        {
            DeviceState deviceState = m_renderer->DeviceState();

            if (deviceState == DeviceState::Playing)
            {
                // Starts a work item to pause playback
                m_renderer->PausePlaybackAsync();
            }
            else if (deviceState == DeviceState::Paused)
            {
                // Starts a work item to pause playback
                m_renderer->StartPlayback();
            }
        }
        else
        {
            StartDevice();
        }
    }















    // Event callback from WASAPI renderer for changes in device state
    fire_and_forget Class::OnRenderDeviceStateChange(IDeviceStateSource const&, WASAPI::DeviceStateChangedEventArgs e)
    {
        auto lifetime = get_strong();

        // Handle state specific messages
        switch (e.DeviceState())
        {
        case DeviceState::Initialized:
            StartDevice();
            if(m_systemMediaControls)
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Closed);
            break;

        case DeviceState::Playing:
            if (m_isMinimumLatency == true)
            {
                //rootPage.NotifyUser(L"Playback Started (minimum latency)", NotifyType::StatusMessage);
            }
            else
            {
                //rootPage.NotifyUser(L"Playback Started (normal latency)", NotifyType::StatusMessage);
            }
            if (m_systemMediaControls)
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Playing);
            break;

        case DeviceState::Paused:
            //rootPage.NotifyUser(L"Playback Paused", NotifyType::StatusMessage);
            if (m_systemMediaControls)
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Paused);
            break;

        case DeviceState::Stopped:
            if (m_deviceStateChangeToken)
            {
                m_renderer->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
            }
            m_renderer = nullptr;

            //rootPage.NotifyUser(L"Playback Stopped", NotifyType::StatusMessage);
            if (m_systemMediaControls)
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Stopped);
            break;

        case DeviceState::Error:
            if (m_deviceStateChangeToken)
            {
                m_renderer->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
            }
            m_renderer = nullptr;

            if (m_systemMediaControls)
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Closed);

            // Specifically handle a couple of known errors
            auto error = e.ExtendedError();
            switch (error)
            {
            case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE:
                //rootPage.NotifyUser(L"ERROR: Endpoint Does Not Support HW Offload", NotifyType::ErrorMessage);
                break;

            case AUDCLNT_E_RESOURCES_INVALIDATED:
                //rootPage.NotifyUser(L"ERROR: Endpoint Lost Access To Resources", NotifyType::ErrorMessage);
                break;

            default:
                //rootPage.NotifyUser(L"ERROR: " + hresult_error(error).message(), NotifyType::ErrorMessage);
                break;
            }
        }

        // Update Control Buttons
        //co_await resume_foreground(Dispatcher());

        UpdateRenderControlUI(e.DeviceState());

        return{};
    }

    
    // Updates transport controls based on current playstate
    void Class::UpdateRenderControlUI(DeviceState deviceState)
    {
        switch (deviceState)
        {
        case DeviceState::Playing:
           /* btnPlay().IsEnabled(false);
            btnStop().IsEnabled(true);
            btnPlayPause().IsEnabled(true);
            btnPause().IsEnabled(true);
            toggleMinimumLatency().IsEnabled(false);*/
            break;

        case DeviceState::Stopped:
        case DeviceState::Error:
            //btnPlay().IsEnabled(true);
            //btnStop().IsEnabled(false);
            //btnPlayPause().IsEnabled(true);
            //btnPause().IsEnabled(false);
            //toggleMinimumLatency().IsEnabled(true);

            //UpdateContentUI();
            break;

        case DeviceState::Paused:
            /*btnPlay().IsEnabled(true);
            btnStop().IsEnabled(true);
            btnPlayPause().IsEnabled(true);
            btnPause().IsEnabled(false);*/
            break;

        case DeviceState::Starting:
        case DeviceState::Stopping:
            /*btnPlay().IsEnabled(false);
            btnStop().IsEnabled(false);
            btnPlayPause().IsEnabled(false);
            btnPause().IsEnabled(false);

            DisableContentUI();*/
            break;
        }
    }


    //
    //   MediaButtonPressed
    //
    fire_and_forget Class::MediaButtonPressed(SystemMediaTransportControls const&, SystemMediaTransportControlsButtonPressedEventArgs e)
    {
        auto lifetime = get_strong();
        //co_await resume_foreground(Dispatcher());

        switch (e.Button())
        {
        case SystemMediaTransportControlsButton::Play:
            StartDevice();
            break;

        case SystemMediaTransportControlsButton::Pause:
            PauseDevice();
            break;

        case SystemMediaTransportControlsButton::Stop:
            StopDevice();
            break;

        default:
            break;
        }
        return {};

    }


    fire_and_forget Class::OnNavigatedTo() //NavigationEventArgs const&)
    {
#if false
        auto lifetime = get_strong();

        // Register for Media Transport controls.  This is required to support background
        // audio scenarios.
        m_systemMediaControls = SystemMediaTransportControls::GetForCurrentView();
        m_systemMediaControlsButtonToken = m_systemMediaControls.ButtonPressed({ get_weak(), &Class::MediaButtonPressed });
        m_systemMediaControls.IsPlayEnabled(true);
        m_systemMediaControls.IsPauseEnabled(true);
        m_systemMediaControls.IsStopEnabled(true);

        //UpdateContentUI();

        // Get a string representing the Default Audio Render Device
        hstring deviceId = MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default);

        // The string is empty if there is no such device.
        if (deviceId.empty())
        {
            //rootPage.NotifyUser(L"No audio devices available", NotifyType::StatusMessage);
            co_return;
        }

        // read property store to see if the device supports a RAW processing mode
        static constexpr wchar_t PKEY_AudioDevice_RawProcessingSupported[] = L"System.Devices.AudioDevice.RawProcessingSupported";
        DeviceInformation deviceInformation = co_await DeviceInformation::CreateFromIdAsync(deviceId, { PKEY_AudioDevice_RawProcessingSupported });

        std::optional<bool> obj = deviceInformation.Properties().TryLookup(PKEY_AudioDevice_RawProcessingSupported).try_as<bool>();
        m_deviceSupportsRawMode = (obj == true);
        //toggleRawAudio().IsEnabled(m_deviceSupportsRawMode);

        if (m_deviceSupportsRawMode)
        {
            //rootPage.NotifyUser(L"Raw Supported", NotifyType::StatusMessage);
        }
        else
        {
            //rootPage.NotifyUser(L"Raw Not Supported", NotifyType::StatusMessage);
        }
#endif

        return{};
    }

    void Class::OnNavigatedFrom()
    {
        StopDevice();

        if (m_deviceStateChangeToken)
        {
            m_renderer->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
        }
        
        if (m_systemMediaControls && m_systemMediaControlsButtonToken)
        {
            m_systemMediaControls.ButtonPressed(m_systemMediaControlsButtonToken);
            m_systemMediaControls.IsPlayEnabled(false);
            m_systemMediaControls.IsPauseEnabled(false);
            m_systemMediaControls.IsStopEnabled(false);
            m_systemMediaControls.PlaybackStatus(MediaPlaybackStatus::Closed);
        }
    }


#pragma endregion

}
