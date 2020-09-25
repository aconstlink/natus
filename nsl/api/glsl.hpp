#pragma once

#include "../typedefs.h"
#include "../parser_structs.hpp"
#include "../generator.hpp"
#include "../dependency_resolver.hpp"

#include <natus/ntd/vector.hpp>

#include <sstream>

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

            enum class glsl_type
            {
                gl3,
                es3
            };

            class generator
            {
                natus_this_typedefs( generator ) ;

            public:

                natus::nsl::generated_code_t generate( natus::nsl::generateable_rref_t dep ) noexcept
                {
                    natus::nsl::generated_code_t ret ;

                    for( auto const& s : dep.config.shaders )
                    {
                        natus::nsl::generated_code_t::code code ;

                        std::stringstream text ;

                        // 1. glsl stuff at the front
                        {
                            text << "#version 130" << std::endl << std::endl ;
                        }

                        // 2. make prototypes declarations from function signatures
                        // the prototype help with not having to sort funk definitions
                        {
                            for( auto const & f : dep.frags )
                            {
                                text << f.sig.return_type << " " ;
                                text << f.sym_long.expand( "_" ) << " ( " ;
                                for( auto const& a : f.sig.args ) text << a + ", " ;
                                text.seekp( -2, std::ios_base::end ) ;
                                text << " ) ; " << std::endl ;
                            }
                            text << std::endl ;
                        }

                        // 3. make all functions with replaced symbols
                        {
                            for( auto const & f : dep.frags )
                            {
                                // start by replacing the function names' symbol itself
                                {
                                    auto const & frag = f.fragments[ 0 ] ;

                                    auto const p0 = frag.find( f.sig.name ) ;
                                    if( p0 == std::string::npos ) continue ;
                                    auto const p1 = frag.find_first_of( ' ', p0 ) ;
                                    text << frag.substr( 0, p0 ) + f.sym_long.expand( "_" ) + frag.substr( p1 ) ;
                                }

                                // then lets go over every symbol in the code
                                for( size_t i=1; i<f.fragments.size(); ++i )
                                {
                                    text << f.fragments[i] ;
                                }
                                text << std::endl ;
                            }
                            text << std::endl ;
                        }

                        // 4. make all glsl uniforms from shader variables
                        {}

                        // 5. insert main/shader from config
                        {
                            for( auto const & c : s.codes )
                            {
                                if( c.versions[ 0 ] != "glsl" ) continue ;
                                
                                for( auto const & l : c.lines )
                                {
                                    text << l << std::endl ;
                                }
                            }
                        }

                        // 6. post over the code and replace all dependencies
                        {
                            auto shd = text.str() ;

                            // variable dependencies
                            {
                                for( auto const& v : dep.vars )
                                {
                                    size_t const p0 = shd.find( v.sym_long.expand() ) ;
                                    if( p0 == std::string::npos ) continue ;
                                    size_t const p1 = shd.find_first_of( " ", p0 ) ;

                                    shd = shd.substr( 0, p0 ) + v.value + shd.substr( p1 ) ;
                                }
                            }

                            // fragment dependencies
                            {
                                for( auto const& f : dep.frags )
                                {
                                    for( auto const& d : f.deps )
                                    {
                                        size_t const p0 = shd.find( d.expand() ) ;
                                        if( p0 == std::string::npos ) continue ;
                                        size_t const p1 = shd.find_first_of( " ", p0 ) ;

                                        shd = shd.substr( 0, p0 ) + d.expand( "_" ) + shd.substr( p1 ) ;
                                    }
                                }
                            }

                            // shader dependencies
                            {
                                for( auto const& d : s.deps )
                                {
                                    size_t const p0 = shd.find( d.expand() ) ;
                                    if( p0 == std::string::npos ) continue ;
                                    size_t const p1 = shd.find_first_of( " ", p0 ) ;

                                    shd = shd.substr( 0, p0 ) + d.expand( "_" ) + shd.substr( p1 ) ;
                                }
                            }
                            code.shader = shd ;
                        }

                        code.api = natus::nsl::api_type::gl3 ;
                        code.type = natus::nsl::shader_type::vertex_shader ;
                        

                        ret.codes.emplace_back( std::move( code ) ) ;
                    }

                    ret.rres = std::move( dep ) ;
                    return std::move( ret ) ;
                }
            };
            natus_typedef( generator ) ;
        }
    }
}