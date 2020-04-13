//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _SNAKEOIL_APPLICATION_RENDER_WINDOW_GLX_GLX_CONTEXT_H_
#define _SNAKEOIL_APPLICATION_RENDER_WINDOW_GLX_GLX_CONTEXT_H_

#include "../irender_context.h"
#include "../iwindow_handle.h"
#include "../gl_info.h"

#include <snakeoil/math/vector/vector4.hpp>
#include <snakeoil/gpu/driver/gl/gl_context.h>

#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glcorearb.h>

#include <list>

namespace so_app
{
    namespace so_xlib
    {
        class glx_context : public irender_context, public so_gpu::so_driver::so_gl::gl_context
        {
            so_this_typedefs( glx_context ) ;

        private:
            
            Display * _display = NULL ;
            Window _wnd ;
            
            GLXContext _context ;
                        
        private:

            /// copy construction is not allowed.
            /// a render context is a unique object.
            glx_context( this_cref_t ) {}

        public:
        
            /// initializes this context but does not create the context.
            /// @see create_context
            glx_context( void_t ) ;

            /// allows to move-construct a context.
            glx_context( this_rref_t ) ;
            virtual ~glx_context( void_t ) ;

        private:

            //glx_context( HWND hwnd, HGLRC ctx ) ;

        public:

            virtual so_app::result activate( void_t ) ;
            virtual so_app::result deactivate( void_t ) ;
            virtual so_app::result vsync( bool_t on_off ) ;
            virtual so_app::result swap( void_t ) ;
            virtual irender_context_ptr_t create_shared_context( void_t ) ;

        public:
            
            so_app::result create_glx( void_t ) ;
            
        public:

            /// @note a valid window handle must be passed.
            so_app::result create_context( Display*, Window, GLXContext ) ;
            
            /// allows to create a shared context for supporting the main context.
            /// this context can not be associated with a separate window.
            this_ptr_t create_shared( void_t ) ;
            void_t destroy_shared( this_ptr_t ) ;

            /// Returns ok, if the extension is supported, otherwise, this function fails.
            /// @precondition Must be used after context has been created and made current.
            so_app::result is_extension_supported( std::string const & extension_name ) ;

            /// This function fills the incoming list with all wgl extension strings.
            /// @precondition Must be used after context has been created and made current.
            so_app::result get_glx_extension( std::list< std::string > & ext_list ) ;

            /// @precondition Context must be current.
            so_app::result get_gl_extension( std::list< std::string > & ext_list ) ;

            /// @precondition Must be used after context has been created and made current. 
            result get_gl_version( so_app::gl_version & version ) const ;

            /// @precondition Context must be active. For debug purpose. Just clears the screen.
            void_t clear_now( so_math::vec4f_t const & vec ) ;

        private:

            so_app::result create_the_context( gl_info_cref_t gli ) ;
            //HGLRC create_the_shared( void_t ) ;

        };
        so_typedefs( glx_context, glx_context ) ;
    }    
}

#endif

