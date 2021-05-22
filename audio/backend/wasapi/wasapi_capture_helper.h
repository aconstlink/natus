#pragma once

#include "../audio_capture_helper.h"

#include <mmdeviceapi.h>
#include <audioclient.h>
#include <windows.h>

namespace natus
{
    namespace audio
    {
        // audio capture of system output/mix using wasapi based on:
        // https://docs.microsoft.com/en-us/windows/win32/coreaudio/loopback-recording
        // https://docs.microsoft.com/en-us/windows/win32/coreaudio/capturing-a-stream
        class NATUS_AUDIO_API wasapi_capture_helper : public natus::audio::audio_capture_helper
        {
            natus_this_typedefs( wasapi_capture_helper ) ;

            typedef std::function< void_t ( BYTE const * buffer, UINT32 const num_frames, natus::ntd::vector< float_t > & ) > copy_funk_t ;
            copy_funk_t _copy_funk ;

        private:

            const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
            const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
            const IID IID_IAudioClient = __uuidof(IAudioClient);
            const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

            // this is in 100 nanosecond units, so this should be 1 second
            // this should be left at 1 second.
            size_t const REFTIMES_PER_SEC = 10000000 ;
            size_t const REFTIMES_PER_MILLISEC  = 10000 ;

            IMMDeviceEnumerator *pEnumerator = NULL ;
            IMMDevice *pDevice = NULL ;
            IAudioClient *pAudioClient = NULL;
            IAudioCaptureClient *pCaptureClient = NULL;

        public:

            wasapi_capture_helper( void_t ) noexcept ;
            wasapi_capture_helper( this_rref_t ) noexcept ;
            virtual ~wasapi_capture_helper( void_t ) noexcept ;

        public:

            virtual bool_t init( natus::audio::channels const, natus::audio::frequency const ) noexcept override ;
            virtual void_t release( void_t ) noexcept override ;
            virtual void_t start( void_t ) noexcept override ;
            virtual void_t stop( void_t ) noexcept override ;
            virtual bool_t capture( natus::ntd::vector< float_t > & samples ) noexcept override ;
        };
        natus_res_typedef( wasapi_capture_helper ) ;
    }
}