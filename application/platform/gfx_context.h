#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../protos.h"
#include "../result.h"

#include <natus/graphics/backend/backend.h>

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
            virtual natus::application::result vsync( bool_t const on_off ) = 0 ;
            virtual natus::application::result swap( void_t ) = 0 ;

        public:

            virtual natus::graphics::backend_res_t create_backend( void_t ) noexcept = 0 ;
        };
        natus_res_typedef( gfx_context ) ;
    }
}