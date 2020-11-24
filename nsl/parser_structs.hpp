
#pragma once

#include "api.h"
#include "typedefs.h"
#include "symbol.hpp"
#include "enums.hpp"

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
                struct lib_function
                {
                    natus::ntd::string_t sig ;
                    natus::ntd::vector< natus::ntd::string_t > body ;
                };
                natus::ntd::vector< lib_function > functions ;
                
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

        namespace post_parse
        {
            struct config
            {
                struct shader
                {
                    struct variable
                    {
                        natus::nsl::flow_qualifier fq ;
                        natus::nsl::type_t type ;
                        natus::ntd::string_t name ;
                        natus::nsl::binding binding ;
                        natus::ntd::string_t line ;
                    };
                    natus_typedef( variable ) ;
                    natus::ntd::vector< variable > variables ;

                    struct code
                    {
                        natus::nsl::language_class version ;
                        natus::ntd::vector< natus::ntd::string_t > lines ;
                    };
                    natus_typedef( code ) ;
                    natus::ntd::vector< code > codes ;

                    symbols_t deps ;

                    shader_type type ;
                };
                natus_typedef( shader ) ;
                natus::ntd::vector< shader > shaders ;

                natus::ntd::string_t name ;
            };
            natus_typedef( config ) ;
            natus_typedefs( natus::ntd::vector< config >, configs ) ;

            struct library
            {
                struct fragment
                {
                    natus::ntd::vector< natus::ntd::string_t > fragments ;

                    
                    natus::nsl::signature_t sig ;

                    symbols_t deps ;

                    // the signature of all used function symbols in the fragment
                    struct dep_signature
                    {
                        natus::ntd::string_t name ;
                        natus::ntd::vector< dep_signature > args ;
                    };
                    natus_typedef( dep_signature ) ;
                    natus::ntd::vector< dep_signature > dep_sigs ;

                    natus::nsl::symbol_t sym_long ;

                    bool_t operator == ( fragment const & other ) const 
                    {
                        if( sym_long != other.sym_long ) return false ;
                        return sig == other.sig ;
                    }
                };
                natus_typedef( fragment ) ;
                natus_typedefs( natus::ntd::vector< fragment >, fragments ) ;
                fragments_t fragments ;

                struct variable
                {
                    natus::nsl::symbol_t sym_long ;

                    natus::nsl::type_t type ;
                    natus::ntd::string_t name ;
                    natus::ntd::string_t value ;
                    natus::ntd::string_t line ;

                    bool_t operator == ( variable const & rhv ) const
                    {
                        return sym_long == rhv.sym_long ;
                    }

                    bool_t operator != ( variable const& rhv ) const
                    {
                        return sym_long != rhv.sym_long ;
                    }

                };
                natus_typedef( variable ) ;
                natus_typedefs( natus::ntd::vector< variable >, variables ) ;

                variables_t variables ;

                // @todo not required
                //natus::ntd::vector< natus::ntd::string_t > names ;
            };
            natus_typedef( library ) ;
            natus_typedefs( natus::ntd::vector< library_t >, libraries ) ;

            struct NATUS_NSL_API document
            {
                configs_t configs ;
                libraries_t libraries ;

                libraries_t find_all( natus::nsl::symbol_cref_t sym ) const noexcept
                {
                    libraries_t ret ;

                    for( auto const & l : libraries )
                    {
                        for( auto const & s : l.fragments )
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