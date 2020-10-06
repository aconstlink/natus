
#pragma once

#include "../typedefs.h"

namespace natus
{
    namespace audio
    {
        enum class capture_type
        {
            undefined,
            what_u_hear,
            microphone
        };

        class capture_object
        {
            natus_this_typedefs( capture_object ) ;


        private:

            capture_type _ct ;

        public:

            capture_object( capture_type const ) 
            {
            }

            capture_object( this_cref_t ) = delete ;
            capture_object( this_rref_t ) 
            {
            }

            virtual ~capture_object( void_t )
            {
            }

        public:



        };
        natus_res_typedef( capture_object ) ;
    }
}