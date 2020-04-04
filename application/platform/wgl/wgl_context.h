#pragma once

#include "../gl_info.h"

#include "../../typedefs.h"
#include "../../result.h"

#include <natus/soil/res.hpp>
#include <natus/math/vector/vector4.hpp>

#include <windows.h>

namespace natus
{
    namespace application
    {
        namespace wgl
        {
            class context
            {
                natus_this_typedefs( context ) ;

            private:

                HWND _hwnd = NULL ;
                HGLRC _hrc = NULL ;

                /// will only be used temporarily for making the
                /// context active. Will be released on deactivation.
                HDC _hdc = NULL ;

            public:

                /// initializes this context but does not create the context.
                /// @see create_context
                context( void_t ) ;
                context( HWND ) ;
                context( this_cref_t ) = delete ;

                /// allows to move-construct a context.
                context( this_rref_t ) ;
                ~context( void_t ) ;

            private:

                context( HWND hwnd, HGLRC ctx ) ;

            public: // operator =

                this_ref_t operator = ( this_cref_t ) = delete ;
                this_ref_t operator = ( this_rref_t ) ;

            public:

                natus::application::result activate( void_t ) ;
                natus::application::result deactivate( void_t ) ;
                natus::application::result vsync( bool_t on_off ) ;
                natus::application::result swap( void_t ) ;

            public:

                /// @note a valid window handle must be passed.
                natus::application::result create_context( HWND hwnd ) ;

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

                natus::application::result create_the_context( natus::application::gl_info_cref_t gli ) ;
            };
            natus_typedef( context ) ;
            typedef natus::soil::res< context_t > context_res_t ;
        }
    }
}