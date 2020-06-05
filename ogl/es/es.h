#pragma once

#include "../result.h"
#include "../api.h"
#include "../typedefs.h"

#if defined( NATUS_TARGET_OS_WIN )

#elif defined( NATUS_TARGET_OS_LIN )
#else
#endif

#include <natus/std/list.hpp>

namespace natus
{
    namespace ogl
    {
        struct NATUS_OGL_API es
        {
            natus_this_typedefs( es ) ;

        public:

            typedef natus::std::list< natus::std::string > string_list_t ;
            static string_list_t _extensions ;

            static bool_t _init ;

        public:

            static natus::ogl::result init( void_t ) ;

            static bool_t is_supported( char const* name ) ;
        };
    }
}
