#pragma once

#include "../typedefs.h"
#include "../parser_structs.hpp"
#include "../generator.hpp"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        namespace glsl
        {
            namespace detail
            {
                static const natus::ntd::vector< natus::ntd::string_t > glsl_types = {
                    "void", "bool", "int", "uint", "float", "double",
                    "vec2", "vec3", "vec4", "mat2", "mat3", "mat4",
                    "dvec2", "dvec3", "dvec4", "dmat2", "dmat3", "dmat4",
                    "sampler1D", "sampler2D", "sampler3D",
                    "isampler1D", "isampler2D", "isampler3D",
                    "uisampler1D", "uisampler2D", "uisampler3D",
                    "image1D", "image2D", "image3D",
                    "iimage1D", "iimage2D", "iimage3D",
                    "samplerCube", "imageCube",
                    "sampler2DRect", "image2DRect"
                    "samplerBuffer", "imageBuffer",
                    "isamplerBuffer", "iimageBuffer",
                    "sampler1DShadow", "sampler2DShadow"
                } ;

                bool_t is_type( natus::ntd::string_cref_t s ) noexcept
                {
                    for( auto const& t : glsl_types )
                    {
                        if( t == s ) return true ;
                    }
                    return false ;
                }
            }

            class function_signature_analyser
            {
                natus::ntd::string_t return_type ;
                natus::ntd::string_t name ;
                natus::ntd::vector< natus::ntd::string_t > arg_types ;

                natus::ntd::vector< natus::ntd::string_t > _tokens ;

            public: 

                function_signature_analyser( natus::ntd::vector< natus::ntd::string_t > const & tokens ) noexcept
                {
                    _tokens = tokens ;
                }

                natus::nsl::post_parse::library_t::fragment_t::signature_t process( void_t ) noexcept
                {
                    natus::nsl::post_parse::library_t::fragment_t::signature_t s ;

                    auto iter_open = std::find( _tokens.begin(), _tokens.end(), "(" ) ;
                    auto iter_close = std::find( _tokens.begin(), _tokens.end(), ")" ) ;

                    if( iter_open == _tokens.end() ) return std::move( s ) ;
                    if( iter_close == _tokens.end() ) return std::move( s ) ;

                    // arg types
                    {
                        auto iter = iter_open ;
                        auto end = ++iter_close ;
                        --iter_close ;

                        natus::ntd::vector< natus::ntd::string_t > arg_ ;
                        while( ++iter != end )
                        {
                            if( *iter == "," || iter == iter_close )
                            {
                                size_t const n = arg_types.size() ;

                                for( auto const & s : arg_ )
                                {
                                    if( detail::is_type( s ) )
                                    {
                                        arg_types.emplace_back( s ) ;
                                        break ;
                                    }
                                }

                                if( arg_types.size() == n )
                                    arg_types.emplace_back( "unknown_type" ) ;

                                arg_.clear() ;
                            }
                            arg_.emplace_back( *iter ) ;
                        }

                        // if arg list is empty, replace with void
                        if( arg_types.size() == 1 && arg_types[ 0 ] == "unknown_type" )
                            arg_types[ 0 ] = "void" ;
                    }

                    
                    // name and return type
                    {
                        if( std::distance( _tokens.begin(), iter_open ) >= 2 )
                        {
                            name = *--iter_open ;
                            return_type = *--iter_open ;
                        }
                        else return std::move( s ) ;
                    }

                    s.args = arg_types ;
                    s.name = name ;
                    s.return_type = return_type ;

                    return std::move( s ) ;
                }
            };

            class glsl_generator
            {
                natus_this_typedefs( glsl_generator ) ;

            public:

                /*natus::nsl::generated_code_t generate( natus::nsl::pregen::document_cref_t doc ) noexcept
                {
                    natus::nsl::generated_code_t ret ;



                    return std::move( ret ) ;
                }*/
            };
            natus_typedef( glsl_generator ) ;
        }
    }
}