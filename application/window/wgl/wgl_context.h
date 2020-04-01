#pragma once

#include "../irender_context.h"
#include "../iwindow_handle.h"
#include "../gl_info.h"

#include <natus/math/vector/vector4.hpp>
//#include <natus/gpu/api/gl/gl_any/gl_context.h>

#include <natus/std/string.hpp>
#include <natus/std/vector.hpp>

#include <windows.h>

namespace natus
{
    namespace application
    {
        namespace win32
        {
            class NATUS_APPLICATION_API wgl_context : public irender_context
                //, public natus_gpu::natus_gl::gl_context
            {
                natus_this_typedefs( wgl_context ) ;

            private:

                HWND _hwnd = NULL ;
                HGLRC _hrc = NULL ;

                /// will only be used temporarily for making the
                /// context active. Will be released on deactivation.
                HDC _hdc = NULL ;

            private:

                /// copy construction is not allowed.
                /// a render context is a unique object.
                wgl_context( this_cref_t ) {}

            public:

                /// initializes this context but does not create the context.
                /// @see create_context
                wgl_context( void_t ) ;

                /// allows to move-construct a context.
                wgl_context( this_rref_t ) ;
                virtual ~wgl_context( void_t ) ;

            private:

                wgl_context( HWND hwnd, HGLRC ctx ) ;

            public: // operator =

                this_ref_t operator = ( this_cref_t ) = delete ;
                this_ref_t operator = ( this_rref_t ) ;

            public:

                static this_ptr_t create( this_rref_t, natus::memory::purpose_cref_t ) ;
                static void_t destroy( this_ptr_t ) ;

            public:

                virtual natus::application::result activate( void_t ) ;
                virtual natus::application::result deactivate( void_t ) ;
                virtual natus::application::result vsync( bool_t on_off ) ;
                virtual natus::application::result swap( void_t ) ;
                virtual irender_context_ptr_t create_shared_context( void_t ) ;

            public:

                /// @note a valid window handle must be passed.
                natus::application::result create_context( gl_info_cref_t gli, iwindow_handle_rptr_t wnd_ptr ) ;

                /// allows to create a shared context for supporting the main context.
                /// this context can not be associated with a separate window.
                this_ptr_t create_shared( void_t ) ;
                void_t destroy_shared( this_ptr_t ) ;

                /// Returns ok, if the extension is supported, otherwise, this function fails.
                /// @precondition Must be used after context has been created and made current.
                natus::application::result is_extension_supported( natus::std::string_cref_t extension_name ) ;

                /// This function fills the incoming list with all wgl extension strings.
                /// @precondition Must be used after context has been created and made current.
                natus::application::result get_wgl_extension( natus::std::vector< natus::std::string_t >& ext_list ) ;

                /// @precondition Context must be current.
                natus::application::result get_gl_extension( natus::std::vector< natus::std::string_t >& ext_list ) ;

                /// @precondition Must be used after context has been created and made current. 
                result get_gl_version( natus::application::gl_version& version ) const ;

                /// @precondition Context must be active. For debug purpose. Just clears the screen.
                void_t clear_now( natus::math::vec4f_t const& vec ) ;

            private:

                HWND get_win32_handle( iwindow_handle_rptr_t ) ;

                natus::application::result create_the_context( gl_info_cref_t gli ) ;
                HGLRC create_the_shared( void_t ) ;

            };
            natus_typedef( wgl_context ) ;
        }
    }
}