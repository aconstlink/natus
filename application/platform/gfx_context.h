#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../protos.h"
#include "../result.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API gfx_context
        {
        public:

            virtual ~gfx_context( void_t ) {}

        public:

            virtual natus::application::result activate( void_t ) = 0 ;
            virtual natus::application::result deactivate( void_t ) = 0 ;
            virtual natus::application::result vsync( bool_t on_off ) = 0 ;
            virtual natus::application::result swap( void_t ) = 0 ;
        };
    }
}