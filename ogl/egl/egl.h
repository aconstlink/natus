#pragma once 

#include "../result.h"
#include "../typedefs.h"
#include "../api.h"

#include <natus/std/vector.hpp>

#include <GL/glcorearb.h>
#include <GL/glx.h>
#include <GL/glxext.h>

namespace natus 
{
    namespace ogl
    {
        struct NATUS_OGL_API glx
        {
            natus_this_typedefs( glx ) ;

        private:

            typedef natus::std::vector< natus::std::string > strings_t ;
            static strings_t _egl_extensions ;

        public: 

            /// will init extensions
            static natus::ogl::result init( Display *, int ) ;


            static bool_t is_supported( char_cptr_t name ) ;
        };
    }
}
