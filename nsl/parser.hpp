

#pragma once

#include "typedefs.h"

#include "ast.hpp"

#include <natus/log/global.h>
#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace nsl
    {
        class parser
        {
            natus_this_typedefs( parser ) ;

        private:

            natus::ntd::string_t _name ;

        public:

            natus_typedefs( natus::ntd::vector< natus::ntd::string_t >, statements ) ;

        public:

            parser( natus::ntd::string_cref_t name ) : _name( name ) {}
            ~parser( void_t ) {}

        public:

            // produces a statement list of all statements in the file
            natus::nsl::ast_t process( natus::ntd::string_cref_t file ) noexcept
            {
                natus::nsl::ast_t ast ;

                auto statements = this_t::scan( file ) ;

                return std::move( ast ) ;
            }

        private: // scan file

            // 1. scans the file content and return a list of all statements in the file.
            // 2. shader content is put into in whole because this is not a GLSL/HLSL/etc parser.
            // 2.1 some shader content will be replaced later on.
            // 3. scopes {} are replaced by <open><close> tags.
            // 4. removes all comments, line breaks, multi spaces
            statements_t scan( natus::ntd::string_cref_t file ) noexcept
            {
                statements_t statements ;

                if( !this_t::check_curlies( file ) )
                {
                    natus::log::global_t::error("[parser] : curly braces not ok for [" + _name + "]" ) ;
                    return statements_t() ;
                }

                size_t ooff = 0 ;
                size_t opos = file.find_first_of( '{' ) ; // scope
                while( opos != std::string::npos ) 
                {
                    natus::ntd::string_t line = this_t::clear_line( file.substr( ooff, opos-ooff ) ) ;

                    statements.push_back( line ) ;
                    statements.push_back( "<open>" ) ;
                    
                    if( line.find( "version" ) == 0 )
                    {
                        ooff = opos + 1 ;

                        // take everything in until the next nsl closing } as content/statement
                        // this need to be searched for because the code inside will have brackets too.
                        {
                            size_t count = 0 ;
                            do {
                                char_t const c = file[ opos++ ] ;
                                count += ( c == '{' ? 1 : ( c == '}' ? -1 : 0 ) ) ;
                            } while( count != 0 ) ;
                        }
                        size_t const dif = ( --opos ) - ooff ;
                        natus::ntd::string_t insert = dif != 0 ? file.substr( ooff, dif ) : " " ;
                        if( dif > 0 ) statements.push_back( this_t::clear_line( std::move( insert ) ) ) ;
                        statements.push_back( "<close>" ) ;
                    }

                    ooff = opos + 1;
                    opos = file.find_first_of( '{', ooff ) ; // scope
                    
                    {
                        size_t spos = file.find_first_of( ';', ooff ) ;
                        while( spos < opos )
                        {
                            statements.push_back( this_t::clear_line( file.substr( ooff, ( spos + 1 ) - ooff ) ) ) ;
                            ooff = spos + 1 ;
                            spos = file.find_first_of( ';', ooff ) ;
                        }
                    }

                    // check if closed is before next open - just drag the offset along
                    {
                        size_t cpos = file.find_first_of( '}', ooff ) ;
                        //if( cpos < opos ) 
                        {
                            //statements.push_back( "<close>" ) ;
                            //ooff = cpos + 1 ;
                            //cpos = file.find_first_of( '}', ooff ) ;
                            while( cpos < opos )
                            {
                                statements.push_back( "<close>" ) ;
                                ooff = cpos + 1 ;
                                cpos = file.find_first_of( '}', ooff ) ;
                            }
                        }
                    }
                }

                return std::move( statements ) ;
            }

        private: // helper

            bool_t check_curlies( natus::ntd::string_cref_t file ) const noexcept
            {
                size_t count = 0 ;
                for( auto const c : file )
                {
                    count = count + ( c == '{' ? 1 : (c == '}' ? -1 : 0 )  ) ;
                }
                return count == 0 ;
            }

            natus::ntd::string_t clear_line( natus::ntd::string_rref_t s ) const noexcept
            {
                if( s.empty() ) return std::move( s ) ;

                s = this_t::remove_comment_lines( std::move( s ) ) ;

                // clear all line breaks
                {
                    for( size_t i = 0; i < s.size(); ++i )
                    {
                        if( s[ i ] == '\r' ) s[ i ] = ' ' ;
                        if( s[ i ] == '\n' ) s[ i ] = ' ' ;
                    }
                }
                
                // clear spaces before and after statement
                {
                    size_t const p0 = s.find_first_not_of( ' ' ) ;
                    size_t const p1 = s.find_last_not_of( ' ' ) ;
                    size_t const dif = p1 - p0 ;
                    if( dif != 0 ) s = s.substr( p0, dif + 1 ) ;
                }

                // clear multi spaces
                {
                    size_t p0 = s.find_first_of( ' ' ) ;
                    while( p0 != std::string::npos )
                    {
                        size_t const p1 = s.find_first_not_of( ' ', p0 ) ;
                        size_t const diff = p1 - p0 ;
                        if( diff > 1 && p1 != std::string::npos )
                        {
                            // +1 : just store one space back for pretty printing
                            s = s.substr( 0, p0 + 1 ) + s.substr( p0 + diff ) ;
                        }
                        p0 = s.find_first_of( ' ', p0 + 1 ) ;
                    }
                }

                return std::move( s ) ;
            }

            natus::ntd::string_t remove_comment_lines( natus::ntd::string_rref_t s ) const noexcept
            {
                size_t off = 0 ;
                size_t p0 = s.find_first_of( "//" ) ;
                while( p0 != std::string::npos )
                {
                    size_t const p1 = s.find_first_of( '\n', p0 ) ;
                    s = s.substr( 0, p0 ) + s.substr( p1 ) ;
                    
                    off = p1 + 1 ;
                    p0 = s.find_first_of( "//", off ) ;
                }

                return std::move( s ) ;
            }
            
        };
        natus_res_typedef( parser ) ;

    }
}