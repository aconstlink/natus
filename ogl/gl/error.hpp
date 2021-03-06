#pragma once

#include "gl.h"
#include "convert.hpp"

#include <natus/ntd/string.hpp>
#include <natus/log/global.h>

namespace natus
{
    namespace ogl
    {
        struct error
        {
            static bool_t check_and_log( natus::ntd::string_in_t msg ) noexcept
            {
                GLenum __so__err = natus::ogl::glGetError() ;
                if( __so__err == GL_NO_ERROR ) return false ;

                std::string const __glstring = natus::ogl::to_string( __so__err ) ;
                natus::log::global_t::error( msg + " " + __glstring ) ;
                return true ;
            }
        };
    }
}