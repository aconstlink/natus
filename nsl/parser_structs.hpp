
#pragma once

#include "typedefs.h"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        namespace parse
        {
            struct config
            {
                struct render_states
                {
                    natus::ntd::vector< natus::ntd::string_t > lines ;
                };
                natus::ntd::vector< render_states > rstates ;

                struct code
                {
                    natus::ntd::vector< natus::ntd::string_t > versions ;
                    natus::ntd::vector< natus::ntd::string_t > lines ;
                };

                struct variable
                {
                    natus::ntd::string_t flow_qualifier ;
                    natus::ntd::string_t type ;
                    natus::ntd::string_t name ;
                    natus::ntd::string_t binding ;
                    natus::ntd::string_t line ;
                };

                struct shader
                {
                    natus::ntd::string_t type ;
                    natus::ntd::vector< variable > variables ;
                    natus::ntd::vector< code > codes ;
                };
                natus::ntd::vector< shader > shaders ;

                natus::ntd::string_t name ;
            };
            natus_typedef( config ) ;
            natus_typedefs( natus::ntd::vector< config_t >, configs ) ;

            struct library
            {
                struct shader
                {
                    natus::ntd::vector< natus::ntd::string_t > versions ;
                    natus::ntd::vector< natus::ntd::string_t > fragments ;
                };
                natus::ntd::vector< shader > shaders ;
                
                struct variable
                {
                    natus::ntd::string_t type ;
                    natus::ntd::string_t name ;
                    natus::ntd::string_t value ;
                    natus::ntd::string_t line ;
                };

                natus::ntd::vector< variable > variables ;
                natus::ntd::vector< natus::ntd::string_t > names ;
            };
            natus_typedef( library ) ;
            natus_typedefs( natus::ntd::vector< library_t >, libraries ) ;
        }

        namespace pregen
        {
            struct library
            {
                struct shader
                {
                    natus::ntd::vector< natus::ntd::string_t > versions ;
                    natus::ntd::vector< natus::ntd::string_t > fragments ;

                    struct signature
                    {
                        natus::ntd::string_t return_type ;
                        natus::ntd::string_t name ;
                        natus::ntd::vector< natus::ntd::string_t > args ;
                    };
                    natus_typedef( signature ) ;
                    signature_t sig ;
                };
                natus_typedef( shader ) ;
                natus::ntd::vector< shader > shaders ;

                struct variable
                {
                    natus::ntd::string_t type ;
                    natus::ntd::string_t name ;
                    natus::ntd::string_t value ;
                    natus::ntd::string_t line ;
                };

                natus::ntd::vector< variable > variables ;
                natus::ntd::vector< natus::ntd::string_t > names ;
            };
            natus_typedef( library ) ;
            natus_typedefs( natus::ntd::vector< library_t >, libraries ) ;
        }
    }
}