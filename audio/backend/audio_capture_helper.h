
#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../result.h"
#include "../enums.h"
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API audio_capture_helper
        {
            natus_this_typedefs( audio_capture_helper ) ;
            typedef natus::memory::res< this_t > this_res_t ;

        public:

            virtual bool_t init( natus::audio::channels const = natus::audio::channels::mono, 
                natus::audio::frequency const = natus::audio::frequency::freq_48k ) noexcept = 0  ;
            virtual void_t release( void_t ) noexcept = 0 ;
            virtual void_t start( void_t ) noexcept = 0 ;
            virtual void_t stop( void_t ) noexcept = 0 ;
            virtual bool_t capture( natus::ntd::vector< float_t > & samples ) noexcept = 0 ;
                
        public:

            // create system specific system audio capture object
            static this_res_t create( void_t ) noexcept ;
            
        };
        natus_res_typedef( audio_capture_helper ) ;
    }
}