#pragma once

#include "../gl_info.h"

#include "../../typedefs.h"
#include "../../result.h"

#include <natus/soil/res.hpp>
#include <natus/math/vector/vector4.hpp>

#include <GL/glcorearb.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glcorearb.h>

namespace natus
{
    namespace application
    {
        namespace glx
        {
            class context
            {
                natus_this_typedefs( context ) ;

            private:

                Display * _display = NULL ;
                Window _wnd ;

                GLXContext _context ;

            public:

                natus_typedefs( 
                     natus::std::vector< natus::std::string >, strings ) ;

            public:

                context( void_t ) ;
                context( gl_info_in_t, Window wnd, Display * disp ) ;
                context( this_cref_t ) = delete ;
                /// allows to move-construct a context.
                context( this_rref_t ) ;
                ~context( void_t ) ;

            private:

                bool_t  determine_gl_version( gl_version & ) const ;

            public:

                natus::application::result activate( void_t ) ;
                natus::application::result deactivate( void_t ) ;
                natus::application::result vsync( bool_t on_off ) ;
                natus::application::result swap( void_t ) ;

            public:

                natus::application::result create_glx( void_t ) ;

            public:

                /// @note a valid window handle must be passed.
                natus::application::result create_context( 
                    Display*, Window, GLXContext ) ;

                /// Returns ok, if the extension is supported, 
                /// otherwise, this function fails.
                /// @precondition Must be used after context has 
                /// been created and made current.
                natus::application::result is_extension_supported( 
                    natus::std::string_cref_t extension_name ) ;

                /// This function fills the incoming list with all 
                /// wgl extension strings.
                /// @precondition Must be used after context has 
                /// been created and made current.
                natus::application::result get_glx_extension( 
                    strings_out_t ext_list ) ;

                /// @precondition Context must be current.
                natus::application::result get_gl_extension( 
                    strings_out_t ext_list ) ;

                /// @precondition Must be used after context has 
                /// been created and made current. 
                natus::application::result get_gl_version( 
                    natus::application::gl_version & version ) const ;

                /// @precondition Context must be active. 
                /// For debug purpose. Just clears the screen.
                void_t clear_now( natus::math::vec4f_t const & vec ) ;

            private:

                natus::application::result create_the_context( 
                    gl_info_cref_t gli ) ;
            };
            natus_typedef( context ) ;
            typedef natus::soil::res< context_t > context_res_t ;
        }
    }
}