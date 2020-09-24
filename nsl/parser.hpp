

#pragma once

#include "typedefs.h"

#include "ast.hpp"
#include "symbol_table.hpp"
#include "parser_structs.hpp"
#include "api/glsl.hpp"

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
            natus::nsl::post_parse::document_t process( natus::ntd::string_rref_t file ) noexcept
            {
                natus::nsl::post_parse::document_t doc ;

                if( !this_t::some_first_checks(file ) ) 
                {
                    return std::move( doc ) ;
                }

                file = this_t::remove_comment_lines( std::move( file ) ) ;
                file = this_t::insert_spaces( std::move( file ) ) ;
                auto statements = this_t::replace_open_close( this_t::scan( std::move( file ) ) ) ;
                
                statements = this_t::repackage( std::move( statements ) ) ;

                // with the statements we can do:
                // 1. sanity checks here possible
                // 2. create symbol table
                // 3. create dependency symbols
                // 4. create ast

                {
                    auto s1 = filter_library_statements( statements_t( statements ) ) ;
                    doc.libraries = analyse_libraries( std::move( s1 ) ) ;
                }
                {
                    auto s1 = filter_config_statements( statements_t( statements ) ) ;
                    doc.configs = analyse_configs( std::move( s1 ) ) ;
                }

                return std::move( doc ) ;
            }

        private: // 

            natus::nsl::parse::configs_t filter_config_statements( this_t::statements_rref_t ss ) const noexcept
            {
                natus::nsl::symbol_table_t st ;

                ss = filter_for_group( "config", std::move( ss ) ) ;

                natus::nsl::parse::configs_t configs ;
                natus::nsl::parse::config_t c ;

                size_t level = 0 ;

                for( auto iter = ss.begin(); iter != ss.end(); ++iter )
                {
                    auto token = this_t::tokenize( *iter ) ;

                    if( token[0] == "open" && token[1] == "config" && level == 0 )
                    {
                        c.name = token[ 2 ] ;
                        ++level ;
                        continue ;
                    }

                    else if( token[ 0 ] == "close" && token[ 1 ] == "config" )
                    {
                        configs.emplace_back( std::move( c ) ) ;
                        --level ;
                        continue ;
                    }

                    // do not check what is not in the config
                    if( level == 0 ) continue ;

                    if( token[ 0 ] == "open" && token[ 1 ] == "render_states" )
                    {
                        level++ ;

                        natus::nsl::parse::config_t::render_states rs ;
                        while( true )
                        {
                            token = this_t::tokenize( *++iter ) ;

                            if( token[ 0 ] == "close" && token[ 1 ] == "render_states" )
                                break ;

                            rs.lines.emplace_back( *iter ) ;
                        }
                        c.rstates.emplace_back( std::move( rs ) ) ;
                        --level ;
                        continue ;
                    }

                    if( token[ 0 ] == "open" && ( token[ 1 ] == "vertex_shader" || token[ 1 ] == "pixel_shader" ) )
                    {
                        ++level ;
                        natus::nsl::parse::config_t::shader s ;
                        s.type = token[ 1 ] ;

                        natus::nsl::parse::config_t::code cod ;
                        bool_t in_shader = false ;
                        while( true )
                        {
                            token = this_t::tokenize( *++iter ) ;

                            if( token.back() == ";" && !in_shader )
                            {
                                natus::nsl::parse::config_t::variable v ;
                                v.line = *iter ;
                                size_t base = 0 ;

                                if( token[ 0 ] == "in" || token[ 0 ] == "out" )
                                {
                                    v.flow_qualifier = token[ 0 ] ;
                                    base = 1 ;
                                }
                                v.type = token[ base + 0 ] ;
                                v.name = token[ base + 1 ] ;
                                if( token.size() > 3 )
                                {
                                    v.binding = token[ base + 3 ] ;
                                }
                                s.variables.emplace_back( std::move( v ) ) ;
                                
                                continue ;
                            }

                            else if( token[0] == "open" && token[1] == "shader" )
                            {
                                in_shader = true ;
                                cod.lines.clear() ;
                                

                                for( size_t i = 2; i < token.size(); ++i )
                                    cod.versions.emplace_back( token[ i ] ) ;
                            }

                            else if( token[ 0 ] == "close" && token[ 1 ] == "shader" )
                            {
                                in_shader = false ;
                                s.codes.emplace_back( std::move( cod ) ) ;
                            }

                            else if( in_shader )
                            {
                                cod.lines.emplace_back( *iter ) ;
                            }

                            if( token[ 0 ] == "close" && token[ 1 ] == s.type )
                                break ;
                            
                            // do analysis
                        }

                        c.shaders.emplace_back( s ) ;

                        --level ;
                    }
                }

                return std::move( configs ) ;
            }
            
            natus::nsl::post_parse::configs_t analyse_configs( natus::nsl::parse::configs_rref_t configs ) const noexcept
            {
                natus::nsl::post_parse::configs_t conf_ret ;
                
                using config = natus::nsl::post_parse::config_t ;
                using shader = natus::nsl::post_parse::config_t::shader_t ;
                using variable = natus::nsl::post_parse::config_t::shader_t::variable_t ;
                using code = shader::code_t ;

                for( auto const & cnf : configs )
                {
                    config c ;
                    c.name = cnf.name ;

                    for( auto const & shd : cnf.shaders )
                    {
                        shader s ;
                        s.type = shd.type ;

                        for( auto const & cd : shd.codes ) 
                        {
                            code c_ ;
                            c_.lines = cd.lines ;
                            c_.versions = cd.versions ;

                            // this symbol depending on
                            {
                                for( auto const& f : cd.lines )
                                {
                                    s.deps = natus::nsl::symbol_t::merge(
                                        std::move( s.deps ), natus::nsl::symbol_t::find_all_symbols( "nsl.", f ) ) ;
                                }
                            }

                            s.codes.emplace_back( std::move( c_ ) ) ;
                        }

                        for( auto const & var : shd.variables )
                        {
                            variable v ;
                            v.binding = var.binding ;
                            v.flow_qualifier = var.flow_qualifier ;
                            v.line = var.line ;
                            v.name = var.name ;
                            v.type = var.type ;
                            s.variables.emplace_back( std::move( v ) ) ;
                        }
                        c.shaders.emplace_back( std::move( s ) ) ;
                    }
                    conf_ret.emplace_back( std::move( c ) ) ;
                }

                return std::move( conf_ret ) ;
            }

            natus::nsl::parse::libraries_t filter_library_statements( this_t::statements_rref_t ss ) const noexcept
            {
                ss = filter_for_group( "library", std::move( ss ) ) ;

                natus::ntd::vector< natus::ntd::string_t > names ;
                natus::nsl::parse::libraries_t libs ;

                // current library
                natus::nsl::parse::library_t lib ;

                bool_t in_shader = false ;

                // 1. coarsely find and differentiate shaders and variables 
                for( size_t i=0; i<ss.size(); ++i )
                {
                    auto const token = this_t::tokenize( ss[i] ) ;
                    
                    if( token[0] == "open" && token[1] == "library" )
                    {
                        names.emplace_back( token[ 2 ] ) ;
                        lib.names = names ;
                    }
                    else if( token[0] == "close" && token[1] == "library" )
                    {
                        names.pop_back() ;
                        libs.emplace_back( std::move( lib ) ) ;
                        lib.names = names ;
                    }
                    else if( token[0] == "open" && token[1] == "shader" )
                    {
                        in_shader = true ;

                        natus::nsl::parse::library_t::shader shd ;
                        
                        for( size_t t = 2; t < token.size(); ++t ) shd.versions.emplace_back( token[ t ] ) ;

                        if( shd.versions.empty() )
                            shd.versions.emplace_back( "version_missing" ) ;

                        while( true )
                        {
                            if( this_t::tokenize( ss[++i] )[ 0 ] == "close" ) break ;
                            shd.fragments.emplace_back( ss[ i ] ) ;
                        }
                        --i ;

                        // split shader with multiple symbols in multiple shaders
                        {
                            size_t level = 0 ;
                            natus::nsl::parse::library_t::shader shd2 = shd ;
                            shd2.fragments.clear() ;

                            for( auto iter = shd.fragments.begin(); iter != shd.fragments.end(); ++iter )
                            {
                                shd2.fragments.emplace_back( *iter ) ;

                                if( *iter == "{" && level++ == 0 )
                                {}

                                if( *iter == "}" && --level == 0 )
                                {
                                    lib.shaders.emplace_back( std::move( shd2 ) ) ;
                                    shd2.versions = shd.versions ;
                                    shd2.fragments.clear() ;
                                }
                            }
                        }
                    }
                    else if( token[0] == "close" && token[1] == "shader" )
                    {
                        in_shader = false ;
                    }
                    else if( token.size() > 4 && !in_shader )
                    {
                        natus::nsl::parse::library_t::variable v ;

                        v.type = token[ 0 ] ;
                        v.name = token[ 1 ] ;
                        v.line = ss[i] ;

                        auto iter = std::find( token.begin(), token.end(), "=" ) ;
                        while( ++iter != token.end() && *iter != ";" )
                        {
                            v.value += *(iter) + " " ;
                        }
                        if( v.value.size() > 0 ) v.value = v.value.substr( 0, v.value.size() - 1 ) ;

                        lib.variables.emplace_back( std::move( v ) ) ;
                    }
                }
                
                return std::move( libs ) ;
            }

            natus::nsl::post_parse::libraries_t analyse_libraries( natus::nsl::parse::libraries_rref_t libs ) const noexcept
            {
                natus::nsl::post_parse::libraries_t lib_ret ;

                using library = natus::nsl::post_parse::library_t ;
                using fragment = natus::nsl::post_parse::library_t::fragment_t ;
                using variable = natus::nsl::post_parse::library_t::variable_t ;

                for( auto& lib : libs )
                {
                    library cur_lib ;

                    // dissect shaders
                    // - for referenceable symbols like functions
                    // - for dependable symbols like function or variables
                    for( auto& shd : lib.shaders )
                    {
                        fragment s ;
                        
                        {
                            s.sym_long = "nsl" ;
                            for( auto const& n : lib.names ) s.sym_long += n  ;
                        }

                        // symbol/function name/signature
                        {
                            auto iter = std::find( shd.versions.begin(), shd.versions.end(), "glsl" ) ;
                            if( iter != shd.versions.end() )
                            {
                                auto const token = this_t::tokenize( shd.fragments[ 0 ] ) ;
                                s.sig = natus::nsl::glsl::function_signature_analyser( token ).process() ;
                                s.sym_long += s.sig.name ;
                            }
                            else
                            {
                                natus::log::global_t::warning("[nsl:parser] : Only GLSL supported but found [" + *iter + "]" ) ;
                                continue ;
                            }
                        }

                        // this symbol depending on
                        {
                            for( auto const& f : shd.fragments )
                            {
                                s.deps = natus::nsl::symbol_t::merge( 
                                    std::move(s.deps), natus::nsl::symbol_t::find_all_symbols( "nsl.", f ) ) ;
                            }
                        }
                        s.fragments = std::move( shd.fragments ) ;
                        s.versions = std::move( shd.versions ) ;
                        cur_lib.fragments.emplace_back( std::move( s ) ) ;
                    }

                    // process variable symbols
                    {
                        for( auto& var : lib.variables )
                        {
                            variable v ;
                            {
                                v.sym_long = "nsl" ;
                                for( auto const& s : lib.names ) v.sym_long += s ;
                            }

                            v.sym_long += var.name ;
                            v.name = var.name ;
                            v.line = var.line ;
                            v.type = var.type ;
                            v.value = var.value ;
                            cur_lib.variables.emplace_back( v ) ;
                        }
                    }

                    lib_ret.emplace_back( std::move( cur_lib ) ) ;
                }

                return std::move( lib_ret ) ;
            }

            statements_t filter_for_group( natus::ntd::string_cref_t what, statements_rref_t ss ) const noexcept
            {
                size_t level = size_t( 0 ) ;
                auto it = ss.begin() ;
                while( it != ss.end() )
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
                    else if( level == 0 ) 
                    {
                        it = ss.erase( it ) ; continue ;
                    }

                    ++it ;
                }

                return std::move( ss ) ;
            }

            // replacing the open/close tags in the statements list makes
            // later processing much easier/less code.
            statements_t replace_open_close( statements_rref_t ss ) const noexcept
            {
                // 1. remove/replace open/close tags
                {
                    natus::ntd::stack< natus::ntd::string_t, 10 > stack ;

                    size_t level = 0 ;

                    for( auto iter = ss.begin(); iter != ss.end(); ++iter )
                    {
                        auto const token = this_t::tokenize( *iter ) ;

                        if( token[0] == "{" )
                        {
                            auto inner_token = this_t::tokenize( *( --iter ) ) ;
                            

                            if( inner_token[ 0 ] != "library" &&
                                inner_token[ 0 ] != "config" && 
                                inner_token[ 0 ] != "shader" && 
                                inner_token[ 0 ] != "render_states" &&
                                inner_token[ 0 ] != "vertex_shader" && 
                                inner_token[ 0 ] != "pixel_shader" ) 
                            {
                                ++level ;
                                ++iter ;
                                continue ;
                            }

                            stack.push( inner_token[0] ) ;

                            // transform <open> to open command
                            *iter = "open" ;
                            for( auto const t : inner_token ) *iter += " " + t ;
                            
                            // remove the open tag
                            iter = --ss.erase( ++iter ) ;
                        }
                        else if( token[0] == "}" )
                        {
                            if( level == 0 )
                                *iter = "close " + stack.pop() ;
                            else --level ;
                        }
                    }
                }
                
                return std::move( ss ) ;
            }


            statements_t repackage( statements_rref_t ss ) const noexcept
            {
                // 1. reunite ()
                {
                    for( auto iter = ss.begin(); iter != ss.end(); ++iter )
                    {
                        if( *iter == "(" )
                        {
                            auto iter2 = --iter + 1 ;
                            while( *iter2 != ")" )
                            {
                                *iter += " " + *iter2 ;
                                iter2 = ss.erase( iter2 ) ;
                            }
                            *iter += " )" ;
                            iter2 = ss.erase( iter2 ) ;

                            iter = iter2 ;
                        }
                    }
                }

                // 2. ; to last line
                {
                    for( auto iter = ss.begin(); iter != ss.end(); ++iter )
                    {
                        if( *iter == ";" )
                        {
                            iter = --ss.erase( iter ) ;
                            *iter += " ;" ;
                        }
                    }
                }

                return std::move( ss ) ;
            }

        private: //

            bool_t some_first_checks( natus::ntd::string_cref_t file ) const noexcept 
            {
                if( !this_t::check_open_close( "{", "}", file ) )
                {
                    natus::log::global_t::error( "[parser] : curly braces not ok for [" + _name + "]" ) ;
                    return false;
                }

                if( !this_t::check_open_close( "/*", "*/", file ) )
                {
                    natus::log::global_t::error( "[parser] : comments not ok [" + _name + "]" ) ;
                    return false;
                }

                if( !this_t::check_open_close( "(", ")", file ) )
                {
                    natus::log::global_t::error( "[parser] : brakets not ok [" + _name + "]" ) ;
                    return false;
                }
                return true ;
            }

            bool_t check_open_close( natus::ntd::string_cref_t open, natus::ntd::string_cref_t close, natus::ntd::string_cref_t file ) const noexcept
            {
                size_t off = 0 ;
                size_t p0 = file.find( open ) ;
                while( p0 != std::string::npos )
                {
                    size_t const p1 = file.find( close, off ) ;

                    if( p1 < p0 ) return false ;
                    else if( p1 == std::string::npos ) return false ;

                    p0 = file.find( open, p1 ) ;
                    off = p0 ;
                }
                return true ;
            }

        private: // scan file

            statements_t scan( natus::ntd::string_rref_t file ) const noexcept
            {
                statements_t statements ;

                // 1. seed statements with the whole file as a single line
                statements.emplace_back( this_t::clear_line( std::move(file) ) ) ;

                // 2. disect for various characters
                {
                    std::function< statements_t ( statements_rref_t, char_t const ) > disect = 
                        [&] ( statements_rref_t statements, char_t const c ) 
                    { 
                        for( auto iter = statements.begin(); iter != statements.end(); ++iter )
                        {
                            size_t pos = ( *iter ).find_first_of( c ) ;
                            while( pos != std::string::npos )
                            {
                                natus::ntd::string_t line = this_t::clear_line(
                                    ( *iter ).substr( 0, pos ) ) ;

                                iter = ++statements.insert( iter, line ) ;
                                iter = ++statements.insert( iter, natus::ntd::string_t( 1, c) ) ;
                                *iter = ( *iter ).substr( pos + 1 ) ; 

                                pos = ( *iter ).find_first_of( c ) ;
                            }
                        }

                        return std::move( statements ) ;
                    } ;
                
                    statements = disect( std::move( statements ), '{' ) ;
                    statements = disect( std::move( statements ), '}' ) ;
                    statements = disect( std::move( statements ), '(' ) ;
                    statements = disect( std::move( statements ), ')' ) ;
                    statements = disect( std::move( statements ), ';' ) ;
                }

                // 3. post process statements for empty lines
                {
                    auto end = std::remove_if( statements.begin(), statements.end(), [&] ( natus::ntd::string_cref_t s )
                    { return s.empty() ; } ) ;

                    statements.erase( end, statements.end() ) ;
                }

                // 4. remove white spaces
                {
                    for( auto & s : statements )
                    {
                        if( s[ 0 ] == ' ' ) s = s.substr( 1 ) ;
                    }
                }

                // 5. remove empty scopes
                {
                    for( auto iter = statements.begin(); iter != statements.end(); ++iter )
                    {
                        auto iter_next = iter + 1 ;
                        if( *iter == ")" && *iter_next == "{" ) 
                        {
                            ++iter ;
                            continue ;
                        }

                        if( *iter == "{" && *iter_next == "}" )
                        {
                            iter = statements.erase( iter ) ;
                            iter = statements.erase( iter ) ;
                            if( *( --iter ) == "{" ) --iter ;
                        }
                    }
                }

                return std::move( statements ) ;
            }

        private: // helper

            // ensures that certain characters have spaces in front or behind.
            // this makes later code analysis much easier and just less character checking.
            natus::ntd::string_t insert_spaces( natus::ntd::string_rref_t s ) const noexcept
            {
                for( auto iter = s.begin(); iter != s.end(); ++iter )
                {
                    if( *iter == '+' || *iter == '-' || *iter == '<' || *iter == '>' )
                    {
                        auto iter_next = iter + 1 ;
                        auto iter_last = iter - 1 ;

                        if( iter_next == iter_last ) continue ;

                        // in front
                        if( *iter != *iter_last ) iter = ++s.insert( iter, ' ' ) ;

                        iter_next = iter + 1 ;

                        // behind
                        if( *iter != *iter_next ) iter = s.insert( ++iter, ' ' ) ;
                    }

                    if( *iter == '(' || *iter == ')' || *iter == ';'  || *iter == ','
                        || *iter == ':' || *iter == '*' || *iter == '/' || *iter == '='  )
                    {
                        // in front
                         iter = ++s.insert( iter, ' ' ) ;
                        // behind
                        iter = s.insert( ++iter, ' ' ) ;
                    }
                }
                return std::move( s )  ;
            }

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
                    if( dif == 0 ) return "" ;
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