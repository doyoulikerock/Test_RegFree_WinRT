#include "pch.h"
#include "Env.h"
#include "Env.g.cpp"

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
    int32_t Env::Prm1() { return prm1; }
    void Env::Prm1(int32_t value) {
        prm1 = value;
    }




}
