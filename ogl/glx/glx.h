//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _SNAKEOIL_GLI_GLX_GLX_H_
#define _SNAKEOIL_GLI_GLX_GLX_H_

#include "../result.h"
#include "../typedefs.h"
#include "../so_api.h"

#include <list>

#include <GL/glcorearb.h>
#include <GL/glx.h>
#include <GL/glxext.h>

namespace so_gli 
{
    namespace so_glx
    {
        struct glx
        {
            so_this_typedefs( glx ) ;

        private:

            typedef std::list< std::string > string_list_t ;
            static string_list_t _glx_extensions ;

        public: 

            /// will initialize all static wgl functions. a context must
            /// be bound in order to get the correct driver .dll where 
            /// all functions are loaded from.
            /// @precondition a opengl context must be current.
            static so_gli::result init( Display *, int ) ;

            static bool_t is_supported( char_cptr_t name ) ;

        private:

            static void_ptr_t load_glx_function( char_cptr_t name ) ;

        public: 
            
            static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs ;
            static PFNGLXSWAPINTERVALEXTPROC glXSwapInterval ;
            static PFNGLXCHOOSEFBCONFIGPROC glXChooseFBConfig ;
            
#if 0
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
            
#endif
        
        };
    }
}

#endif

