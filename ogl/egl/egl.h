#pragma once 

#include "../result.h"
#include "../typedefs.h"
#include "../api.h"

#include <natus/std/vector.hpp>

//#include <GL/glcorearb.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace natus 
{
    namespace ogl
    {
        struct NATUS_OGL_API egl
        {
            natus_this_typedefs( egl ) ;

        private:

            typedef natus::std::vector< natus::std::string > strings_t ;
            static strings_t _egl_extensions ;

        public: 

            /// will init extensions
            static natus::ogl::result init( EGLNativeDisplayType ) ;


            static bool_t is_supported( char_cptr_t name ) ;
        };
    }
}
