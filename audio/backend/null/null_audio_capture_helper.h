
#pragma once

#include "../audio_capture_helper.h"

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API null_audio_capture_helper : public audio_capture_helper
        {
            natus_this_typedefs( null_audio_capture_helper ) ;

        public:

            virtual bool_t init( void_t ) noexcept override { return true ; }
            virtual void_t release( void_t ) noexcept override {}
            virtual void_t start( void_t ) noexcept override {} ;
            virtual void_t stop( void_t ) noexcept override {} ;
            virtual bool_t capture( natus::ntd::vector< float_t > & samples ) noexcept override { return true ; }
        
        };
        natus_res_typedef( null_audio_capture_helper ) ;
    }
}