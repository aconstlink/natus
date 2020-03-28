//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _SNAKEOIL_APPLICATION_RENDER_WINDOW_GLX_GLX_WINDOW_H_
#define _SNAKEOIL_APPLICATION_RENDER_WINDOW_GLX_GLX_WINDOW_H_

#include "glx_context.h"

#include "../render_window.h"
#include "../xlib/xlib_window.h"

#include "../gl_info.h"
#include "../window_info.h"

#include <snakeoil/gpu/driver/gl/gl_driver.h>

namespace so_app
{
    namespace so_xlib
    {
        /// Allows to render GL into the window. It constructs a xlib window
        /// internally and sets it as the decorated window.
        /// Furthermore, it creates a glx context which allows calling the GL.
        /// With the context, a GL driver object is created that can be directly
        /// used for rendering with using the engine.
        class glx_window : public render_window
        {
            typedef render_window base_t ;
            so_this_typedefs( glx_window ) ;

            typedef so_gpu::so_driver::so_gl::gl_driver_ptr_t gl_driver_ptr_t ;

            typedef std::vector< gl_driver_ptr_t > support_drivers_t ;
            typedef std::vector< glx_context_ptr_t > support_contexts_t ;

        private:

            /// the rendering context using the glx.
            glx_context_ptr_t _context_ptr = nullptr ;

            /// the engine's GL driver for using engine
            /// objects for rendering.
            gl_driver_ptr_t _driver_ptr = nullptr ;

            support_contexts_t _support_contexts ;
            support_drivers_t _support_drivers ;
            
        public:

            glx_window( gl_info_cref_t gli, window_info_cref_t wi ) ;
            glx_window( this_rref_t ) ;
            virtual ~glx_window( void_t ) ;

        public:

            static this_ptr_t create( this_rref_t rhv ) ;
            static void_t destroy( this_ptr_t ptr ) ;
                        
        public: // interface 

            virtual so_app::result destroy( void_t ) ;
            virtual irender_context_ptr_t get_render_context( void_t ) ;
            virtual render_window::driver_ptr_t get_driver( void_t ) ;
            
            virtual irender_context_ptr_t get_support_context( size_t i ) ;
            virtual render_window::driver_ptr_t get_support_driver( size_t i ) ;

        protected:

            so_app::result create_context( void_t ) ;
            
            Window create_glx_window( Display * display, gl_info_cref_t gli, window_info_cref_t wi ) ;
            
        };
    }    
}

#endif

