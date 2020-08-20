#pragma once

#include "../result.h"
#include "../typedefs.h"
#include "../api.h"

#include <natus/ntd/vector.hpp>
#include <natus/ntd/string.hpp>

#include <windows.h>
#include <GL/glcorearb.h>
#include <GL/wglext.h>

namespace natus 
{
    namespace ogl
    {
        struct NATUS_OGL_API wgl
        {
            natus_this_typedefs( wgl ) ;

        private:

            typedef natus::ntd::vector< natus::ntd::string_t > strings_t ;
            static strings_t _wgl_extensions ;

        public: 

            /// will initialize all static wgl functions. a context must
            /// be bound in order to get the correct driver .dll where 
            /// all functions are loaded from.
            /// @precondition a opengl context must be current.
            static natus::ogl::result init( HDC hdc ) ;

            static bool_t is_supported( char_cptr_t name ) ;

        private:

            static void_ptr_t load_wgl_function( char_cptr_t name ) ;

        public: 

            static PFNWGLCREATEBUFFERREGIONARBPROC wglCreateBufferRegion;
            static PFNWGLDELETEBUFFERREGIONARBPROC wglDeleteBufferRegion;
            static PFNWGLSAVEBUFFERREGIONARBPROC wglSaveBufferRegion;
            static PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegion;

            static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs;

            static PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsString;
            static PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrent;
            static PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDC;

            static PFNWGLCREATEPBUFFERARBPROC wglCreatePbuffer;
            static PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDC;
            static PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDC;
            static PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbuffer;
            static PFNWGLQUERYPBUFFERARBPROC wglQueryPbuffer;

            static PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribiv;
            static PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfv;
            static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormat;
            static PFNWGLBINDTEXIMAGEARBPROC wglBindTexImage;
            static PFNWGLRELEASETEXIMAGEARBPROC wglReleaseTexImage;
            static PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttrib;
            static PFNWGLSETSTEREOEMITTERSTATE3DLPROC wglSetStereoEmitterState3D;


        public: // ext

            static PFNWGLSWAPINTERVALEXTPROC wglSwapInterval ;
            static PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapInterval ;
        };
    }
}