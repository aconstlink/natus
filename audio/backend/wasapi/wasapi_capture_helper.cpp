
#include "wasapi_capture_helper.h"

using namespace natus::audio ;

//*************************************************************************
wasapi_capture_helper::wasapi_capture_helper( void_t ) noexcept 
{
}

//*************************************************************************
wasapi_capture_helper::wasapi_capture_helper( this_rref_t ) noexcept
{
}

//*************************************************************************
wasapi_capture_helper::~wasapi_capture_helper( void_t ) noexcept 
{
    this_t::release() ;
}

//*************************************************************************
bool_t wasapi_capture_helper::init( void_t ) noexcept 
{
    this_t::release() ;

    {
        auto const res = CoInitializeEx( NULL, COINIT_MULTITHREADED ) ;
        // is S_FALSE if already initialized
        /*if( res != S_OK )
        {
            natus::log::global::error( natus_log_fn( "CoInitializeEx" ) ) ;
            return 1 ;
        }*/
    }

    {
        HRESULT const hr = CoCreateInstance(
            CLSID_MMDeviceEnumerator, NULL,
            CLSCTX_ALL, IID_IMMDeviceEnumerator,
            (void**)&pEnumerator ) ;

        if( hr != S_OK )
        {
            natus::log::global::error( natus_log_fn( "CoCreateInstance") ) ;
            return false ;
        }
    }

    {
        auto const res = pEnumerator->GetDefaultAudioEndpoint( eRender, eConsole, &pDevice ) ;

        if( res != S_OK )
        {
            natus::log::global::error( natus_log_fn("GetDefaultAudioEndpoint") ) ;
            return false ;
        }
    }

    {
        auto const res = pDevice->Activate( IID_IAudioClient, CLSCTX_ALL,
            NULL, (void**)&pAudioClient );

        if( res != S_OK )
        {
            natus::log::global::error( natus_log_fn( "Device Activate" ) ) ;
            return false ;
        }
    }

    {
        // format see: mmreg.h
        WAVEFORMATEX *pwfx = NULL ;
        WAVEFORMATEXTENSIBLE * fmt_ext = NULL ;

        //IAudioClient *pAudioClient = NULL;
        
        
        //REFERENCE_TIME hnsActualDuration;

        copy_funk_t copy_funk = [=]( BYTE const * buffer, UINT32 const num_frames, natus::ntd::vector< float_t > & ){} ;
        
        {
            auto const res = pAudioClient->GetMixFormat( &pwfx ) ;
            if( res != S_OK )
            {
                natus::log::global::error( natus_log_fn( "GetMixFormat" ) ) ;
                return false ;
            }

            if( pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE )
            {
                fmt_ext = (WAVEFORMATEXTENSIBLE*)pwfx ;

                if( fmt_ext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM )
                {
                    // create integral type buffer
                    if( pwfx->wBitsPerSample == 8 )
                    {
                        copy_funk = [=]( BYTE const * buffer, UINT32 const num_frames, natus::ntd::vector< float_t > & samples )
                        {
                            size_t const start = samples.size() ;
                            size_t const end = start + num_frames ;
                            samples.resize( end ) ;
                            for( size_t i=start; i<end; ++i )
                            {
                                samples[i] = float_t(double_t(int8_cptr_t(buffer)[i])/125.0) ;
                            }
                        } ;
                    }
                    else if( pwfx->wBitsPerSample == 16 )
                    {
                        copy_funk = [=]( BYTE const * buffer, UINT32 const num_frames, natus::ntd::vector< float_t > & samples )
                        {
                            size_t const start = samples.size() ;
                            size_t const end = start + num_frames ;
                            samples.resize( end ) ;
                            for( size_t i=start; i<end; ++i )
                            {
                                samples[i] = float_t(double_t(int16_cptr_t(buffer)[i])/32768.0) ;
                            }
                        } ;
                    }
                    else if( pwfx->wBitsPerSample == 32 )
                    {
                        copy_funk = [=]( BYTE const * buffer, UINT32 const num_frames, natus::ntd::vector< float_t > & samples )
                        {
                            size_t const start = samples.size() ;
                            size_t const end = start + num_frames ;
                            samples.resize( end ) ;
                            for( size_t i=start; i<end; ++i )
                            {
                                samples[i] = float_t(double_t(int32_cptr_t(buffer)[i])/2147483648.0) ;
                            }
                        } ;
                    }
                }
                else if( fmt_ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT )
                {
                    copy_funk = [=]( BYTE const * buffer, UINT32 const num_frames, natus::ntd::vector< float_t > & samples )
                    {
                        size_t const start = samples.size() ;
                        size_t const end = start + num_frames ;
                        samples.resize( end ) ;
                        for( size_t i=start; i<end; ++i )
                        {
                            samples[i] = float_cptr_t(buffer)[i] ;
                        }
                    } ;
                }
            }
        }

        _copy_funk = copy_funk ;

        {
            REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC ;
            auto const res = pAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED,
                    AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, pwfx, NULL ) ;

            if( res != S_OK )
            {
                natus::log::global::error( natus_log_fn( "AudioClient Initialize" ) ) ;
                return false ;
            }
        }

        {
            UINT32 bufferFrameCount ;
            auto const res = pAudioClient->GetBufferSize( &bufferFrameCount ) ;
            if( res != S_OK )
            {
                natus::log::global::error( natus_log_fn( "GetBufferSize" ) ) ;
                return false ;
            }
        }

        {
            auto const res = pAudioClient->GetService( IID_IAudioCaptureClient, (void**)&pCaptureClient ) ;
            if( res != S_OK )
            {
                natus::log::global::error( natus_log_fn( "AudioClient GetService" ) ) ;
                return false ;
            }
        }
    }

    {
        auto const res = pAudioClient->Start() ;
        if( res != S_OK )
        {
            natus::log::global::error( natus_log_fn("AudioClient GetService" ) ) ;
            return false ;
        }
    }

    //size_t const num_bits_per_sample = pwfx->wBitsPerSample ;

    return true ;
}

//*************************************************************************
void_t wasapi_capture_helper::release( void_t ) noexcept 
{
    _copy_funk = [=]( BYTE const * buffer, UINT32 const num_frames, natus::ntd::vector< float_t > & ){} ;

    if( pAudioClient )
    {
        pAudioClient->Stop() ;

        pAudioClient->Release() ;
        pAudioClient = nullptr ;
    }

    if( pCaptureClient )
    {
        pCaptureClient->Release() ;
        pCaptureClient = nullptr ;
    }

    if( pEnumerator ) 
    {
        pEnumerator->Release() ;
        pEnumerator = nullptr ;
    }
}

//*************************************************************************
bool_t wasapi_capture_helper::capture( natus::ntd::vector< float_t > & samples ) noexcept 
{
    UINT32 packetLength = 0 ;

    // read available data
    {
        auto const res = pCaptureClient->GetNextPacketSize( &packetLength ) ;
        if( res != S_OK )
        {
            natus::log::global::error( natus_log_fn( "GetNextPacketSize" ) ) ;
            return false;
        }
        if( packetLength == 0 ) return false ;
    }

    while( packetLength != 0 )
    {
        // get loopback data
        BYTE *data_ptr ;
        UINT32 num_frames_available ;
        DWORD flags ;

        {
            auto const res = pCaptureClient->GetBuffer( &data_ptr,
                &num_frames_available, &flags, NULL, NULL ) ;

            if( res != S_OK )
            {
                natus::log::global::error( natus_log_fn( 
                    "unable to get audio buffer. Will cancel thread." ) ) ;
                break ;
            }
        }

        _copy_funk( data_ptr, num_frames_available, samples ) ;

        // release wasapi buffer
        {
            auto const res = pCaptureClient->ReleaseBuffer( num_frames_available ) ;
            if( res != S_OK )
            {
                natus::log::global::error( natus_log_fn( "ReleaseBuffer" ) ) ;
                break ;
            }
        }

        // read available data
        {
            auto const res = pCaptureClient->GetNextPacketSize( &packetLength ) ;
            if( res != S_OK )
            {
                natus::log::global::error( natus_log_fn( "GetNextPacketSize" ) ) ;
                break ;
            }
        }
    }

    return true ;
}

//*************************************************************************