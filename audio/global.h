

#pragma once

#include "api.h"
#include "typedefs.h"

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API global
        {
            natus_this_typedefs( global ) ;

        private: // singleton stuff

            static this_ptr_t _singleton ;
            static this_ptr_t get( void_t ) ;

        private:

        public:

            global( void_t ) ;
            global( this_cref_t ) = delete ;
            global( this_rref_t ) ;
            ~global( void_t ) ;

            static void_t deinit( void_t ) noexcept ;

        private:

        };
        natus_typedef( global ) ;
    }
}