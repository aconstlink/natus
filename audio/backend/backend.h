#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../result.h"

#include "../object/capture_object.h"

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API backend
        {
        
        public:

            virtual natus::audio::result configure( natus::audio::capture_object_res_t ) noexcept = 0 ;

            virtual natus::audio::result capture( natus::audio::capture_object_res_t, bool_t ) noexcept = 0 ;

        public:

            virtual void_t begin( void_t ) noexcept = 0 ;
            virtual void_t end( void_t ) noexcept = 0 ;
        };
        natus_res_typedef( backend ) ;
    }
}