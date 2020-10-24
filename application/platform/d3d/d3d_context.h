#pragma once

#include "../gl_info.h"

#include "../gfx_context.h"

#include <natus/memory/res.hpp>
#include <natus/math/vector/vector4.hpp>

#include <windows.h>

namespace natus
{
    namespace application
    {
        namespace d3d
        {
            class NATUS_APPLICATION_API context : public gfx_context
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

                virtual natus::application::result activate( void_t ) ;
                virtual natus::application::result deactivate( void_t ) ;
                virtual natus::application::result vsync( bool_t const on_off ) ;
                virtual natus::application::result swap( void_t ) ;

            public:

                /// @note a valid window handle must be passed.
                natus::application::result create_context( HWND hwnd ) ;

                /// @precondition Context must be active. For debug purpose. Just clears the screen.
                void_t clear_now( natus::math::vec4f_t const& vec ) ;

            private:

                natus::application::result create_the_context( natus::application::gl_info_cref_t gli ) ;
            };
            natus_typedef( context ) ;
            typedef natus::memory::res< context_t > context_res_t ;
        }
    }
}