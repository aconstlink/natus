#pragma once

#include "wgl_context.h"

#include "../decorator_window.h"
#include "../win32/win32_window.h"

#include "../gl_info.h"
#include "../window_info.h"

//#include <natus/gpx/window/iwindow.h>
//#include <natus/gpu/api/gl/gl_33/gl_33_api.h>

namespace natus
{
    namespace application
    {
        namespace win32
        {
            /// Allows to render GL into the window. It constructs a win32 window
            /// internally and sets it as the decorated window.
            /// Furthermore, it creates a WGL context which allows calling the GL.
            /// With the context, a GL driver object is created that can be directly
            /// used for rendering with using the engine.
            class NATUS_APPLICATION_API wgl_window : public decorator_window//, public natus_gpx::iwindow
            {
                typedef render_window base_t ;
                natus_this_typedefs( wgl_window ) ;

                //typedef natus::gpu::natus_driver::natus_gl::gl_driver_ptr_t gl_driver_ptr_t ;

                //typedef natus::std::vector< natus_gpu::natus_gl::igl_api_ptr_t > support_drivers_t ;
                typedef natus::std::vector< wgl_context_ptr_t > support_contexts_t ;

            private:

                /// the rendering context using the wgl.
                wgl_context_ptr_t _context_ptr = nullptr ;

                bool_t _tgl_vsync = false ;
                bool_t _vsync = false ;

            private:

                //natus::gpu::natus_gl::gl_33_api_ptr_t _gl_api_ptr = nullptr ;

            public:

                wgl_window( gl_info_cref_t gli, window_info_cref_t wi ) ;
                wgl_window( this_rref_t ) ;
                virtual ~wgl_window( void_t ) ;

            public:

                static this_ptr_t create( this_rref_t rhv ) ;
                static void_t destroy( this_ptr_t ptr ) ;

            public: // interface 

                virtual natus::application::result destroy( void_t ) ;
                virtual irender_context_ptr_t get_render_context( void_t ) ;
                //virtual natus_gpu::iapi_ptr_t get_driver( void_t ) ;

                virtual irender_context_ptr_t get_support_context( size_t i ) ;
                //virtual natus_gpu::iapi_ptr_t get_support_driver( size_t i ) ;

                virtual void_t send_toggle( natus::application::toggle_window_in_t ) ;

            public: // natus_gpu interface

                virtual void_t render_thread_begin( void_t ) ;
                virtual void_t render_thread_end( void_t ) ;
                virtual void_t end_frame( void_t ) ;
                //virtual natus_gpu::iapi_ptr_t get_api( void_t ) ;
                //virtual natus_gpu::api_type get_api_type( void_t ) ;

            protected:

                natus::application::result create_context( void_t ) ;
            };
            natus_typedef( wgl_window ) ;
        }
    }
}