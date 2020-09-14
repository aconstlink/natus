

#pragma once

#include "typedefs.h"

#include "ast.hpp"
#include "symbol_table.hpp"

#include <natus/log/global.h>
#include <natus/ntd/vector.hpp>
#include <natus/ntd/stack.hpp>

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

                if( !this_t::some_first_checks(file ) ) 
                {
                    return std::move( ast ) ;
                }

                auto statements = this_t::replace_open_close( 
                    this_t::scan( natus::ntd::string_t( file ) ) ) ;
                
                // with the statements we can do:
                // 1. sanity checks here possible
                // 2. create symbol table
                // 3. create dependency symbols
                // 4. create ast
                
                auto symbols = this_t::tokenize_symbols( statements ) ;
              

                return std::move( ast ) ;
            }

        private: // 

            natus::nsl::symbol_table_t tokenize_symbols( this_t::statements_cref_t ss ) const noexcept
            {
                auto s1 = tokenize_library( statements_t( ss ) ) ;
                auto s2 = tokenize_config( statements_t( ss ) ) ;

                return natus::nsl::symbol_table_t() ;
            }

            natus::nsl::symbol_table_t tokenize_config( this_t::statements_rref_t ss ) const noexcept
            {
                natus::nsl::symbol_table_t st ;

                ss = filter_for_group( "config", std::move( ss ) ) ;

                return std::move( st ) ;
            }

            natus::nsl::symbol_table_t tokenize_library( this_t::statements_rref_t ss ) const noexcept
            {
                natus::nsl::symbol_table_t st ;

                ss = filter_for_group( "library", std::move( ss ) ) ;

                struct shader
                {
                    natus::ntd::vector< natus::ntd::string_t > library ;
                    natus::ntd::vector< natus::ntd::string_t > versions ;
                    natus::ntd::vector< natus::ntd::string_t > fragments ;
                };

                struct context
                {
                    natus::ntd::string_t opcode ;
                    natus::ntd::string_t name ;
                };
                natus::ntd::vector< context > context_stack ;
                
                for( size_t i=0; i<ss.size(); ++i )
                {
                    auto const token = this_t::tokenize( ss[i] ) ;
                    
                    if( token[0] == "open" && token[1] == "library" )
                    {
                        context_stack.emplace_back( context( { token[ 1 ], token[ 2 ] } ) ) ;
                    }
                    else if( token[0] == "close" && token[1] == "library" )
                    {
                        context_stack.pop_back() ;
                    }
                    else if( token[0] == "open" && token[1] == "shader" )
                    {
                        natus::ntd::vector< natus::ntd::string_t > library ;
                        for( auto const & c : context_stack )
                        {
                            library.emplace_back( c.name ) ;
                        }
                        natus::ntd::vector< natus::ntd::string_t > versions ;
                        for( size_t t = 2; t < token.size(); ++t ) versions.emplace_back( token[ t ] ) ;

                        natus::ntd::vector< natus::ntd::string_t > fragments ;
                        while( true )
                        {
                            if( this_t::tokenize( ss[++i] )[ 0 ] == "close" ) break ;
                            fragments.emplace_back( ss[ i ] ) ;
                        }
                        --i ;
                    }
                    else if( token[0] == "close" && token[1] == "shader" )
                    {}
                    else
                    {
                        
                        if( token[0] == "float" )
                        {
                            natus::ntd::string_t ns ;
                            for( auto const & c : context_stack )
                            {
                                
                            }
                            //natus::nsl::symbol_table_t::symbol_t s( ) ;
                        }
                    }

                    int bp = 0 ;
                }

                return std::move( st ) ;
            }

            statements_t filter_for_group( natus::ntd::string_cref_t what, statements_rref_t ss ) const noexcept
            {
                size_t level = size_t( 0 ) ;
                for( auto it = ss.begin(); it < ss.end(); ++it )
                {
                    auto const token = this_t::tokenize( *it ) ;

                    if( token[ 0 ] == "open" && token[ 1 ] == what )
                    {
                        ++level ;
                    }
                    else if( token[ 0 ] == "close" && token[ 1 ] == what )
                    {
                        --level ;
                    }
                    else if( level == 0 ) it = --ss.erase( it ) ;
                }

                return std::move( ss ) ;
            }

            // replacing the open/close tags in the statements list makes
            // later processing much easier/less code.
            statements_t replace_open_close( statements_rref_t ss ) const noexcept
            {
                // 1. replace open/close tags in shaders
                // - replace shader code open/close by {/}
                {
                    size_t level = size_t(-1) ;
                    for( size_t i=0; i<ss.size(); ++i )
                    {
                        auto const token = this_t::tokenize( ss[i] ) ;

                        if( token[0] == "shader" )
                        {
                            ++i ; // jump over the shaders' open tag
                            level = 0 ;
                        }
                        else if( token[0] == "<open>" && level != size_t(-1) )
                        {
                            ss[i] = "{" ;
                            ++level ;
                        }
                        else if( token[0] == "<close>" && level != size_t(-1) )
                        {
                            if( --level != size_t( -1 ) )
                                ss[ i ] = "}" ;
                        }
                    }
                }

                // 2. remove/replace open/close tags
                {
                    natus::ntd::stack< natus::ntd::string_t, 10 > stack ;

                    for( auto iter = ss.begin(); iter != ss.end(); ++iter )
                    {
                        auto const token = this_t::tokenize( *iter ) ;

                        if( token[0] == "<open>" )
                        {
                            auto inner_token = this_t::tokenize( *( --iter ) ) ;
                            stack.push( inner_token[0] ) ;

                            // transform <open> to open command
                            *iter = "open" ;
                            for( auto const t : inner_token ) *iter += " " + t ;
                            
                            // remove the open tag
                            iter = --ss.erase( ++iter ) ;
                        }
                        else if( token[0] == "<close>" )
                        {
                            *iter = "close " + stack.pop() ;
                        }
                    }
                }
                
                return std::move( ss ) ;
            }

        private: //

            bool_t some_first_checks( natus::ntd::string_cref_t file ) const noexcept 
            {
                if( !this_t::check_curlies( file ) )
                {
                    natus::log::global_t::error( "[parser] : curly braces not ok for [" + _name + "]" ) ;
                    return false;
                }

                if( !this_t::check_comments( file ) )
                {
                    natus::log::global_t::error( "[parser] : comments not ok [" + _name + "]" ) ;
                    return false;
                }
                return true ;
            }

            bool_t check_curlies( natus::ntd::string_cref_t file ) const noexcept
            {
                size_t off = 0 ;
                size_t p0 = file.find( "{" ) ;
                while( p0 != std::string::npos )
                {
                    size_t const p1 = file.find( "}", off ) ;

                    if( p1 < p0 ) return false ; 
                    else if( p1 == std::string::npos ) return false ;

                    p0 = file.find( "{", p1 ) ;
                    off = p0 ;
                }
                return true ;
            }

            bool_t check_comments( natus::ntd::string_cref_t file ) const noexcept
            {
                size_t off = 0 ;
                size_t p0 = file.find( "/*" ) ;
                while( p0 != std::string::npos )
                {
                    size_t const p1 = file.find( "*/", off ) ;

                    if( p1 < p0 ) return false ; 
                    else if( p1 == std::string::npos ) return false ;

                    p0 = file.find( "/*", p1 ) ;
                    off = p0 ;
                }
                return true ;
            }

        private: // scan file

            // 1. scans the file content and returns a list of all statements in the file.
            // 2. shader content is parsed too but must be similar to nsl like GLSL/HLSL/etc.
            // 2.1 some shader content will be replaced later on.
            // 2.2 comments in shaders are removed though
            // 3. scopes {} are replaced by <open><close> tags.
            // 4. removes all comments, line breaks, multi spaces
            // 5. checked if all opened curlies are closed
            statements_t scan( natus::ntd::string_rref_t file ) const noexcept
            {
                statements_t statements ;

                file = this_t::remove_comment_lines( std::move( file ) ) ;

                size_t ooff = 0 ;
                size_t opos = file.find_first_of( '{' ) ; // scope
                while( opos != std::string::npos ) 
                {
                    natus::ntd::string_t line = this_t::clear_line( file.substr( ooff, opos-ooff ) ) ;

                    statements.push_back( line ) ;
                    statements.push_back( "<open>" ) ;

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
                        while( cpos < opos )
                        {
                            statements.push_back( "<close>" ) ;
                            ooff = cpos + 1 ;
                            cpos = file.find_first_of( '}', ooff ) ;
                        }
                    }
                }

                return std::move( statements ) ;
            }

        private: // helper

            

            natus::ntd::string_t clear_line( natus::ntd::string_rref_t s ) const noexcept
            {
                if( s.empty() ) return std::move( s ) ;

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

            // simply removes all comments with
            // // and /**/
            natus::ntd::string_t remove_comment_lines( natus::ntd::string_rref_t s ) const noexcept
            {
                // 1. clear all //
                {
                    size_t off = 0 ;
                    size_t p0 = s.find( "//" ) ;
                    while( p0 != std::string::npos )
                    {
                        size_t const p1 = s.find_first_of( '\n', p0 ) ;
                        s = s.substr( 0, p0 ) + s.substr( p1 ) ;

                        off = p0 ;
                        p0 = s.find( "//", off ) ;
                    }
                }

                // 2. clear all /**/
                {
                    size_t off = 0 ;
                    size_t p0 = s.find( "/*" ) ;
                    while( p0 != std::string::npos )
                    {
                        size_t const p1 = s.find( "*/", p0 ) ;
                        s = s.substr( 0, p0 ) + s.substr( p1+2 ) ;

                        p0 = s.find( "/*", p0 ) ;
                    }
                }

                return std::move( s ) ;
            }

            natus::ntd::vector< natus::ntd::string_t > tokenize( natus::ntd::string_cref_t s ) const noexcept
            {
                natus::ntd::vector< natus::ntd::string_t > tokens ;

                size_t off = 0 ;
                size_t pos = s.find_first_of( ' ' ) ;
                while( pos != std::string::npos )
                {
                    tokens.emplace_back( s.substr( off, pos - off ) ) ;

                    off = pos + 1 ;
                    pos = s.find_first_of( ' ', off ) ;
                }
                tokens.emplace_back( s.substr( off ) ) ;

                return std::move( tokens ) ;
            }
            
        };
        natus_res_typedef( parser ) ;

    }
}