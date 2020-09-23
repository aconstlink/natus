
#pragma once

#include "typedefs.h"
#include "symbol.hpp"

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
            struct config
            {
                struct shader
                {
                    struct variable
                    {
                        natus::ntd::string_t flow_qualifier ;
                        natus::ntd::string_t type ;
                        natus::ntd::string_t name ;
                        natus::ntd::string_t binding ;
                        natus::ntd::string_t line ;
                    };
                    natus_typedef( variable ) ;
                    natus::ntd::vector< variable > variables ;

                    struct code
                    {
                        natus::ntd::vector< natus::ntd::string_t > versions ;
                        natus::ntd::vector< natus::ntd::string_t > lines ;
                    };
                    natus_typedef( code ) ;
                    natus::ntd::vector< code > codes ;

                    natus::ntd::vector< natus::ntd::string_t > deps ;

                    natus::ntd::string_t type ;
                };
                natus_typedef( shader ) ;
                natus::ntd::vector< shader > shaders ;

                natus::ntd::string_t name ;
            };
            natus_typedef( config ) ;
            natus_typedefs( natus::ntd::vector< config >, configs ) ;

            struct library
            {
                struct shader
                {
                    natus::ntd::vector< natus::ntd::string_t > lib_names ;
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

                    natus::ntd::vector< natus::ntd::string_t > deps ;

                    natus::ntd::string_t sym_long ;
                };
                natus_typedef( shader ) ;
                natus::ntd::vector< shader > shaders ;

                struct variable
                {
                    natus::ntd::vector< natus::ntd::string_t > lib_names ;
                    natus::ntd::string_t sym_long ;

                    natus::ntd::string_t type ;
                    natus::ntd::string_t name ;
                    natus::ntd::string_t value ;
                    natus::ntd::string_t line ;
                };
                natus_typedef( variable ) ;

                natus::ntd::vector< variable > variables ;
                natus::ntd::vector< natus::ntd::string_t > names ;
            };
            natus_typedef( library ) ;
            natus_typedefs( natus::ntd::vector< library_t >, libraries ) ;

            struct document
            {
                configs_t configs ;
                libraries_t libraries ;

                libraries_t find_all( natus::nsl::symbol_cref_t sym ) const noexcept
                {
                    libraries_t ret ;

                    for( auto const & l : libraries )
                    {
                        for( auto const & s : l.shaders )
                        {
                            if( sym == s.sym_long )
                            {
                                ret.emplace_back( l ) ;
                            }
                        }

                        // if shader found, it can not be a variable anymore
                        if( ret.size() > 0 ) continue ;

                        for( auto const& v : l.variables )
                        {
                            if( sym == v.sym_long )
                            {
                                ret.emplace_back( l ) ;
                            }
                        }
                    }

                    return std::move( ret ) ;
                }
            };
            natus_typedef( document ) ;
        }
    }
}