
#include "audio_capture_helper.h"

#include "null/null_audio_capture_helper.h"

#if defined( NATUS_TARGET_OS_WIN )
#include "wasapi/wasapi_capture_helper.h"
#endif

using namespace natus::audio ;

audio_capture_helper_t::this_res_t natus::audio::audio_capture_helper_t::create( void_t ) noexcept 
{
    audio_capture_helper_res_t ret = null_audio_capture_helper_res_t(
        null_audio_capture_helper_t() ) ;

    #if defined( NATUS_TARGET_OS_WIN )
    ret = wasapi_capture_helper_res_t( wasapi_capture_helper_t() ) ;
    natus::log::global_t::status( "Creating wasapi audio capture helper" ) ;
    #else
    natus::log::global_t::status( "No defaul system audio capture available." ) ;
    #endif

    return ret ;
}