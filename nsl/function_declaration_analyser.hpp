
#include "typedefs.h"
#include "parser_structs.hpp"
#include "generator_structs.hpp"
#include "dependency_resolver.hpp"

namespace natus
{
    namespace nsl
    {
        class function_declaration_analyser
        {
            using signature_t = natus::nsl::post_parse::library_t::fragment_t::signature_t ;

            natus::nsl::type_t return_type ;
            natus::ntd::string_t name ;
            natus::ntd::vector< signature_t::arg_t > _args ;

            natus::ntd::vector< natus::ntd::string_t > _tokens ;

        public:

            function_declaration_analyser( natus::ntd::vector< natus::ntd::string_t > const& tokens ) noexcept
            {
                _tokens = tokens ;
            }

            natus::nsl::post_parse::library_t::fragment_t::signature_t process( void_t ) noexcept
            {
                
                signature_t s ;

                auto iter_open = std::find( _tokens.begin(), _tokens.end(), "(" ) ;
                auto iter_close = std::find( _tokens.begin(), _tokens.end(), ")" ) ;

                if( iter_open == _tokens.end() ) return std::move( s ) ;
                if( iter_close == _tokens.end() ) return std::move( s ) ;

                // arg types
                {
                    auto beg = iter_open ;
                    auto iter = iter_open ;
                    auto end = ++iter_close ;
                    --iter_close ;

                    size_t i = 0 ;
                    while( ++iter != end )
                    {
                        if( *iter == "," || iter == iter_close )
                        {
                            size_t const num_parts = std::distance( beg, iter ) - 1 ;

                            if( num_parts != 2 )
                            {
                                // error. args must type and name only.
                                natus::log::global_t::error( "[parser] : arg must type and name only. Found: " +
                                    *iter ) ;

                                _args.emplace_back( signature_t::arg_t() ) ;
                            }
                            else 
                            {
                                signature_t::arg_t arg ;
                                arg.name = *( iter - 1 ) ;
                                arg.type = natus::nsl::to_type( *( iter - 2 ) ) ;
                                _args.emplace_back( std::move( arg ) ) ;
                            }

                            beg = iter ;
                        }
                    }
                }

                // name and return type
                {
                    if( std::distance( _tokens.begin(), iter_open ) >= 2 )
                    {
                        name = *--iter_open ;
                        return_type = natus::nsl::to_type( *--iter_open ) ;
                    }
                    else return std::move( s ) ;
                }

                s.args = _args ;
                s.name = name ;
                s.return_type = return_type ;

                return std::move( s ) ;
            }
        };
    }
}