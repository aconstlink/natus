#pragma once

#include "../protos.h"
#include "../result.h"
#include "../typedefs.h"
#include "../api.h"

#include "render_context_info.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API irender_context
        {
        public:

            virtual natus::application::result activate( void_t ) = 0 ;
            virtual natus::application::result deactivate( void_t ) = 0 ;

            /// enabnatus::application disables the vsync
            virtual natus::application::result vsync( bool_t on_off ) = 0 ;

            /// swapnatus::applicationbuffers. May wait for vsync if enabled.
            virtual natus::application::result swap( void_t ) = 0 ;

            /// creanatus::applicationhared context that allows you to perform
            /// multi-threaded rendering operations.
            virtual irender_context_ptr_t create_shared_context( void_t ) = 0 ;
        };
    }
}
