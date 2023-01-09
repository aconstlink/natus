
#include "parser.h"

#include "function_declaration_analyser.hpp"

#include <natus/log/global.h>
#include <natus/ntd/stack.hpp>

#include <algorithm>
#include <regex>

using namespace natus::nsl ;

natus::nsl::post_parse::document_t parser::process( natus::ntd::string_cref_t file ) noexcept
{
    return this_t::process( natus::ntd::string_t( file ) ) ;
}

// produces a statement list of all statements in the file
natus::nsl::post_parse::document_t parser::process( natus::ntd::string_rref_t file ) noexcept
{
    natus::nsl::post_parse::document_t doc ;

    file = this_t::remove_comment_lines( std::move( file ) ) ;

    if( !this_t::some_first_checks( file ) )
    {
        return std::move( doc ) ;
    }

    file = this_t::insert_spaces( std::move( file ) ) ;
    auto statements = this_t::replace_open_close( this_t::scan( std::move( file ) ) ) ;

    statements = this_t::repackage( std::move( statements ) ) ;
    statements = this_t::replace_numbers( std::move( statements ) ) ;
    statements = this_t::replace_operators( std::move( statements ) ) ;
    statements = this_t::replace_buildins( std::move( statements ) ) ;

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

natus::nsl::parse::configs_t parser::filter_config_statements( this_t::statements_rref_t ss ) const noexcept
{
    ss = filter_for_group( "config", std::move( ss ) ) ;

    natus::nsl::parse::configs_t configs ;
    natus::nsl::parse::config_t c ;

    size_t level = 0 ;

    for( auto iter = ss.begin(); iter != ss.end(); ++iter )
    {
        auto token = this_t::tokenize( *iter ) ;

        if( token[ 0 ] == "open" && token[ 1 ] == "config" && level == 0 )
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


            while( true )
            {
                token = this_t::tokenize( *++iter ) ;

                if( token.back() == ";" )
                {
                    natus::nsl::parse::config_t::variable v ;
                    v.line = *iter ;
                    size_t base = 0 ;

                    if( token[ 0 ] == "in" || token[ 0 ] == "out" || token[ 0 ] == "inout" )
                    {
                        v.flow_qualifier = token[ 0 ] ;
                        base = 1 ;
                    }
                    v.type = token[ base + 0 ] ;
                    v.name = token[ base + 1 ] ;
                    if( ( token.size() >= 4 + base ) && token[ base + 2 ] == ":" )
                    {
                        v.binding = token[ base + 3 ] ;
                    }
                    s.variables.emplace_back( std::move( v ) ) ;

                    continue ;
                }
                // this should be a function
                else if( *( iter + 1 ) == "{" )
                {
                    natus::nsl::parse::config_t::code cod ;
                    cod.versions.emplace_back( "nsl" ) ;

                    size_t ilevel = 0 ;

                    while( true )
                    {
                        if( *iter == "{" ) ++ilevel ;
                        else if( *iter == "}" ) --ilevel ;

                        if( *iter == "}" && ilevel == 0 )
                            break ;

                        cod.lines.emplace_back( *iter++ ) ;
                    }
                    cod.lines.emplace_back( *iter ) ;

                    s.codes.emplace_back( std::move( cod ) ) ;
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

natus::nsl::post_parse::configs_t parser::analyse_configs( natus::nsl::parse::configs_rref_t configs ) const noexcept
{
    natus::nsl::post_parse::configs_t conf_ret ;

    using config = natus::nsl::post_parse::config_t ;
    using shader = natus::nsl::post_parse::config_t::shader_t ;
    using variable = natus::nsl::post_parse::config_t::shader_t::variable_t ;
    using code = shader::code_t ;

    for( auto const& cnf : configs )
    {
        config c ;
        c.name = cnf.name ;

        for( auto const& shd : cnf.shaders )
        {
            shader s ;
            s.type = natus::nsl::to_shader_type( shd.type ) ;

            for( auto const& cd : shd.codes )
            {
                for( auto const& version : cd.versions )
                {
                    code c_ ;
                    c_.lines = cd.lines ;
                    c_.version = natus::nsl::to_language_class( version ) ;

                    // this symbol depending on
                    {
                        for( auto const& f : cd.lines )
                        {
                            s.deps = natus::nsl::symbol_t::merge(
                                std::move( s.deps ), natus::nsl::symbol_t::find_all_symbols( "nsl.", f ) ) ;
                        }
                    }
                    // determine used buildins
                    {
                        c_.buildins = this_t::determine_used_buildins( cd.lines ) ;
                    }
                    s.codes.emplace_back( std::move( c_ ) ) ;
                }
            }

            for( auto const& var : shd.variables )
            {
                variable v ;
                v.binding = natus::nsl::to_binding( var.binding ) ;
                v.fq = natus::nsl::flow_qualifier_from_binding( natus::nsl::to_flow_qualifier( var.flow_qualifier ), v.binding ) ;
                v.line = var.line ;
                v.name = var.name ;
                v.type = natus::nsl::to_type( var.type ) ;
                if( v.fq == natus::nsl::flow_qualifier::inout )
                {
                    {
                        v.fq = natus::nsl::flow_qualifier::in ;
                        s.variables.emplace_back( v ) ;
                    }
                    {
                        v.fq = natus::nsl::flow_qualifier::out ;
                        s.variables.emplace_back( v ) ;
                    }
                }
                else s.variables.emplace_back( std::move( v ) ) ;
            }
            c.shaders.emplace_back( std::move( s ) ) ;
        }
        conf_ret.emplace_back( std::move( c ) ) ;
    }

    return std::move( conf_ret ) ;
}

natus::nsl::parse::libraries_t parser::filter_library_statements( this_t::statements_rref_t ss ) const noexcept
{
    ss = filter_for_group( "library", std::move( ss ) ) ;

    natus::ntd::vector< natus::ntd::string_t > names ;
    natus::nsl::parse::libraries_t libs ;
    natus::ntd::stack< natus::nsl::parse::library_t, 10 > libs_stack ;

    // current library
    natus::nsl::parse::library_t lib ;

    size_t level = size_t( 0 ) ;

    bool_t in_shader = false ;

    // 1. coarsely find and differentiate shaders and variables 
    for( size_t i = 0; i < ss.size(); ++i )
    {
        auto const token = this_t::tokenize( ss[ i ] ) ;

        if( token[ 0 ] == "open" && token[ 1 ] == "library" )
        {
            libs_stack.push( lib ) ;

            lib = natus::nsl::parse::library_t() ;
            names.emplace_back( token[ 2 ] ) ;
            lib.names = names ;

            ++level ;
        } else if( token[ 0 ] == "close" && token[ 1 ] == "library" )
        {
            --level ;
            libs.emplace_back( std::move( lib ) ) ;

            lib = libs_stack.pop() ;

            names.pop_back() ;
        }
        // nsl shader / variable
        else
        {
            // is function
            if( token.back() != ";" && ss[ i + 1 ] == "{" )
            {
                size_t level_ = 0 ;
                size_t const beg = i ;
                while( ++i != ss.size() )
                {
                    if( ss[ i ] == "{" ) ++level_ ;
                    else if( ss[ i ] == "}" ) --level_ ;

                    if( level_ == 0 ) break ;
                }

                // make shader per function symbol 
                {
                    natus::nsl::parse::library_t::lib_function shd ;
                    shd.sig = ss[ beg ] ;
                    for( size_t j = beg + 2; j < i; ++j )
                    {
                        shd.body.emplace_back( ss[ j ] ) ;
                    }
                    lib.functions.emplace_back( std::move( shd ) ) ;
                }
            }
            // must be a variable
            else if( token.size() > 4 )
            {
                natus::nsl::parse::library_t::variable v ;

                v.type = token[ 0 ] ;
                v.name = token[ 1 ] ;
                v.line = ss[ i ] ;

                auto iter = std::find( token.begin(), token.end(), "=" ) ;
                while( ++iter != token.end() && *iter != ";" )
                {
                    v.value += *( iter ) +" " ;
                }
                if( v.value.size() > 0 ) v.value = v.value.substr( 0, v.value.size() - 1 ) ;

                lib.variables.emplace_back( std::move( v ) ) ;
            } else
            {
                // should be error I guess.
            }
        }
    }

    return std::move( libs ) ;
}

parser::statements_t parser::replace_numbers( statements_rref_t ss ) const
{
    for( auto& s : ss )
    {
        s = std::regex_replace( s, std::regex( " ([0-9]+) " ), " __int__ ( $1 ) " ) ;
        s = std::regex_replace( s, std::regex( " ([0-9]+)u+ " ), " __uint__ ( $1 ) " ) ;
        s = std::regex_replace( s, std::regex( " ([0-9]+)\\.([0-9]+)f? " ), " __float__ ( $1 , $2 ) " ) ;
    }
    return std::move( ss ) ;
}

parser::statements_t parser::replace_operators( statements_rref_t ss ) const
{
    // operators
    // * / + - = 
    // *= /= += -= 
    // < > 
    // <= >= != ==
    // << >> 
    // { } ( ) , ; .

    struct repl
    {
        natus::ntd::string_t what ;
        natus::ntd::string_t with ;

        // a + b -> false
        // a /= b -> false
        // ++a -> true
        // ^b -> true
        bool_t single = false ;
    };

    typedef std::function< bool_t ( char const t, size_t const l ) > is_stop_t ;
    auto do_replacement = [] ( natus::ntd::string_ref_t line, repl const& r, is_stop_t is_stop )
    {
        size_t off = 0 ;
        while( true )
        {
            size_t const p0 = line.find( r.what, off ) ;
            if( p0 == std::string::npos ) break ;
            
            natus::ntd::string_t arg0, arg1 ;

            size_t beg = 0 ;
            size_t end = 0 ;

            // arg 0, left of what
            if( p0 > 0 )
            {
                size_t level = 0 ;
                size_t const cut = p0 - 1 ;
                size_t p1 = p0 ;
                while( --p1 != size_t( -1 ) )
                {
                    if( line[ p1 ] == ')' ) ++level ;
                    else if( line[ p1 ] == '(' ) --level ;

                    if( is_stop( line[ p1 ], level ) ) break ;
                }
                // if the beginning is hit, there is one position missing.
                if( p1 == size_t( -1 ) ) --p1 ;

                // p3 is the (at stop char) + the char + a white space
                size_t const p3 = p1 + 2 ;

                // the condition may happen if the algo above has no arg0 for operator found
                if( p3 > cut ) arg0 = "" ;
                else arg0 = line.substr( p1 + 2, cut - ( p1 + 2 ) ) ;

                beg = p3 ;
            }

            // arg1, right of what
            {
                size_t level = 0 ;

                size_t p1 = p0 + r.what.size() - 1 ;
                while( ++p1 != line.size() )
                {
                    if( line[ p1 ] == '(' ) ++level ;
                    else if( line[ p1 ] == ')' ) --level ;

                    if( is_stop( line[ p1 ], level ) ) break ;
                }

                // +1 : jump over the whitespace
                size_t const cut = p0 + r.what.size() + 1 ;
                if( p1 == cut ) arg1 = "" ;
                else arg1 = line.substr( cut, ( p1 - 1 ) - cut ) ;

                end = p1 ;
            }

            if( r.single )
            {
                // prefix
                if( arg0.empty() )
                {
                    line = line.replace( beg, end - beg,
                        r.with + " ( " + arg1 + " ) " ) ;
                }
                // postfix
                else if( arg1.empty() )
                {
                    line = line.replace( beg, end - beg,
                        r.with.substr(0, r.with.size()-1) + "_post: ( " + arg0 + " ) " ) ;
                }
                // else ambiguous operator 
                else {}
            }
            else
            {
                if( arg0.empty() )
                {
                    line = line.replace( beg, end - beg,
                        r.with + " ( " + arg1 + " ) " ) ;
                }
                else if( arg1.empty() )
                {
                    line = line.replace( beg, end - beg,
                        r.with + " ( " + arg0 + " ) " ) ;
                }
                else
                {
                    line = line.replace( beg, end - beg,
                        r.with + " ( " + arg0 + " , " + arg1 + " ) " ) ;
                }
            }

            // find another
            off = p0 + 1 ;
        }
    } ;

    // replacing operators #0
    {
        natus::ntd::vector< repl > repls =
        {
            //{ "=", "ass" }, // declaration need to be handled first. float_t c = ...
            { "return", ":ret:" }
        } ;

        auto is_stop = [&] ( char const t, size_t const l )
        {
            //if( l == size_t( -1 ) )  return true ;
            //if( l != 0 ) return false ;
            if( t == ';' ) return true ;
            return false ;
        } ;

        for( auto const& r : repls )
        {
            for( auto& line : ss )
            {
                do_replacement( line, r, is_stop ) ;
            }
        }
    }

    // replacing operators #1
    {
        natus::ntd::vector< repl > repls =
        {
            //{ "=", "ass" }, // declaration need to be handled first. float_t c = ...
            { "*=", ":mul_asg:" },
            { "/=", ":div_asg:" },
            { "+=", ":add_asg:" },
            { "-=", ":sub_asg:" },
            { "<=", ":leq:" },
            { ">=", ":geq:" },
            { "!=", ":neq:" },
            { "==", ":eqeq:" }
        } ;

        auto is_stop = [&] ( char const t, size_t const l )
        {
            if( l == size_t( -1 ) )  return true ;
            if( l != 0 ) return false ;
            if( t == ';' || t == ',' ) return true ;
            return false ;
        } ;

        for( auto& line : ss )
        {
            for( auto const& r : repls )
            {
                do_replacement( line, r, is_stop ) ;
            }
        }
    }

    // replacing operators #2
    {
        natus::ntd::vector< repl > repls =
        {
            { "++", ":inc:", true },
            { "--", ":dec:", true },
            { "+", ":plus:", true },
            { "-", ":minus:", true },
            { "*", ":mmul:" },    // math multiplication
            { "'", ":cmul:" },    // component-wise multiplication
            { "/", ":div:" },
            { "+", ":add:" },
            { "-", ":sub:" },
            { ">>", ":rs:" },
            { "<<", ":ls:" },
            { "<", ":lt:" },
            { ">", ":gt:" },
            { "||", ":lor:" },
            { "&&", ":land:" }
        } ;

        auto is_stop = [&] ( char const t, size_t const l )
        {
            if( l == size_t( -1 ) )  return true ;
            if( l != 0 ) return false ;
            if( t == '*' || t == '/' || t == '+' || t == '-' ||
                t == '=' || t == ';' || t == ',' || t == '>' || t == '<' ) return true ;
            return false ;
        } ;

        for( auto & line : ss )
        {
            for( auto const & r : repls )
            {
                do_replacement( line, r, is_stop ) ;
            }
        }
    }

    return std::move( ss ) ;
}


parser::statements_t parser::replace_buildins( statements_rref_t ss ) const 
{
    for( size_t i=0; i<size_t(buildin_type::num_build_ins); ++i )
    {
        auto const bi = natus::nsl::get_build_in( buildin_type(i) ) ;
        if( bi.t == buildin_type::unknown ) continue ;

        for( auto iter = ss.begin(); iter != ss.end(); ++iter )
        {
            size_t p0 = iter->find( bi.fname ) ;
            while( p0 != std::string::npos )
            {
                size_t const p1 = iter->find_first_of( '(', p0 ) ;
                bool_t const a = p0 + bi.fname.size() + 1 == p1 ; 
                bool_t const b = (*iter)[p0 + bi.fname.size()] == ' ' ;
                bool_t const c = p0 == 0 || (p0 != 0 && (*iter)[p0 - 1] == ' ') ;

                if( a && b && c )
                {
                    iter->replace( p0, (p1-p0) - 1, bi.opcode ) ;
                }
                p0 = iter->find( bi.fname, p1 ) ;
            }
        }
    }

    return std::move( ss ) ;
}

natus::nsl::post_parse::libraries_t parser::analyse_libraries( natus::nsl::parse::libraries_rref_t libs ) const noexcept
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
        // - also flattens by language class
        for( auto& shd : lib.functions )
        {
            fragment s ;

            {
                s.sym_long = "nsl" ;
                for( auto const& n : lib.names ) s.sym_long += n  ;
            }

            // symbol/function name/signature
            {
                auto const token = this_t::tokenize( shd.sig ) ;
                s.sig = natus::nsl::function_declaration_analyser( token ).process() ;
                s.sym_long += s.sig.name ;
            }

            // this symbol depending on
            {
                for( auto const& f : shd.body )
                {
                    s.deps = natus::nsl::symbol_t::merge(
                        std::move( s.deps ), natus::nsl::symbol_t::find_all_symbols( "nsl.", f ) ) ;
                }
            }

            // determine used buildins
            {
                s.buildins = this_t::determine_used_buildins( shd.body ) ;
            }

            s.fragments = shd.body ;
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
                v.type = natus::nsl::to_type( var.type ) ;
                v.value = var.value ;
                cur_lib.variables.emplace_back( v ) ;
            }
        }

        lib_ret.emplace_back( std::move( cur_lib ) ) ;
    }

    return std::move( lib_ret ) ;
}

parser::statements_t parser::filter_for_group( natus::ntd::string_cref_t what, statements_rref_t ss ) const noexcept
{
    size_t level = size_t( 0 ) ;
    auto it = ss.begin() ;
    while( it != ss.end() )
    {
        auto const token = this_t::tokenize( *it ) ;

        if( token[ 0 ] == "open" && token[ 1 ] == what )
        {
            ++level ;
        } else if( token[ 0 ] == "close" && token[ 1 ] == what )
        {
            --level ;
        } else if( level == 0 )
        {
            it = ss.erase( it ) ; continue ;
        }

        ++it ;
    }

    return std::move( ss ) ;
}

// replacing the open/close tags in the statements list makes
// later processing much easier/less code.
parser::statements_t parser::replace_open_close( statements_rref_t ss ) const noexcept
{
    // 1. remove/replace open/close tags
    {
        natus::ntd::stack< natus::ntd::string_t, 10 > stack ;

        size_t level = 0 ;

        for( auto iter = ss.begin(); iter != ss.end(); ++iter )
        {
            auto const token = this_t::tokenize( *iter ) ;

            if( token[ 0 ] == "{" )
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

                stack.push( inner_token[ 0 ] ) ;

                // transform <open> to open command
                *iter = "open" ;
                for( auto const t : inner_token ) *iter += " " + t ;

                // remove the open tag
                iter = --ss.erase( ++iter ) ;
            } else if( token[ 0 ] == "}" )
            {
                if( level == 0 )
                    *iter = "close " + stack.pop() ;
                else --level ;
            }
        }
    }

    return std::move( ss ) ;
}


parser::statements_t parser::repackage( statements_rref_t ss ) const noexcept
{
    // 1. reunite ()
    {
        for( auto iter = ss.begin(); iter != ss.end(); ++iter )
        {
            if( *iter == "(" )
            {
                size_t level = 0 ;
                auto iter2 = --iter + 1 ;
                do
                {
                    if( *iter2 == "(" ) ++level ;
                    else if( *iter2 == ")" ) --level ;

                    if( level == 0 ) break ;

                    *iter += " " + *iter2 ;
                    iter2 = ss.erase( iter2 ) ;

                } while( true ) ;

                *iter += " )" ;
                iter2 = ss.erase( iter2 ) ;

                iter = iter2 ;
            }
        }
    }

    // 2. repackage array construction
    {
        for( auto iter = ss.begin(); iter != ss.end(); ++iter )
        {
            // at the moment, only = {...} arrays are supported.
            // arrays like {...}[num] is not supported
            if( (*iter).back() != '=' || *(iter+1) != "{" ) continue ;            
            
            {
                auto const tokens = this_t::tokenize( (*iter).substr( 0, iter->size() - 2 ) ) ;
                *iter = ":make_array: ( " ;
                *iter += tokens[0] + " , " ;
                *iter += tokens[1] + " , " ;
            }
            
            auto iter0 = ss.erase( iter + 1 ) ; // first argument
            auto iter1 = iter ; // running iter
            while( *(++iter1) != "}" )
            {
                if( (*iter1).size() >= 2 )
                {
                    size_t count = 0 ; 
                    count += (*iter1)[0] == ',' ? 1 : 0 ;
                    count += (*iter1)[1] == ' ' ? 1 : 0 ;
                    if( count != 0 ) *iter1 = (*iter1).substr( count, (*iter1).size() - count ) ;
                }
            }

            // append arguments
            {
                size_t const num_args = iter1 - iter0 ;
                *iter += std::to_string( num_args ) + " , " ;
                for( auto i = iter0; i<iter1; ++i )
                {
                    *iter += *i + " , " ;
                }
                // remove trailing ,
                *iter = iter->substr( 0, iter->size() - 2 ) ;
            }

            *iter += " ) " ;
            iter = ss.erase( iter0, iter1 + 1 ) ;
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

                while( *( iter - 1 ) != "{" &&
                    *( iter - 1 ) != "}" &&
                    ( iter - 1 )->find( ';' ) == std::string::npos &&
                    ( iter - 1 )->find( "open " ) == std::string::npos )
                {
                    *( iter - 1 ) += " " + *iter ;
                    iter = ss.erase( iter ) ;
                }
            }
        }
    }

    // 3. merge ) and next line
    {
        for( auto iter = ss.begin(); iter != ss.end(); ++iter )
        {
            if( ( *iter ).back() == ')' && *( iter + 1 ) != "{" )
            {
                *iter += " " + *( iter + 1 ) ;
                iter = ss.erase( ++iter ) ;
            }
        }
    }

    return std::move( ss ) ;
}

bool_t parser::some_first_checks( natus::ntd::string_cref_t file ) const noexcept
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

bool_t parser::check_open_close( natus::ntd::string_cref_t open, natus::ntd::string_cref_t close, natus::ntd::string_cref_t file ) const noexcept
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

parser::statements_t parser::scan( natus::ntd::string_rref_t file ) const noexcept
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

            // lets use this opportunity to remove empty lines.
            if( iter->empty() )
            {
                iter = statements.erase( iter ) ;
                if( iter != statements.begin() ) --iter ;
            }
        }
    }

    return std::move( statements ) ;
}


// ensures that certain characters have spaces in front or behind.
// this makes later code analysis much easier and just less character checking.
natus::ntd::string_t parser::insert_spaces( natus::ntd::string_rref_t s ) const noexcept
{
    for( auto iter = s.begin(); iter != s.end(); ++iter )
    {
        if( *iter == '+' || *iter == '-' || *iter == '*' || *iter == '/' || 
            *iter == '<' || *iter == '>' || *iter == '/' || *iter == '!' || 
            *iter == '=' )
        {
            auto iter_next = iter + 1 ;
            auto iter_last = iter - 1 ;

            if( iter_next == iter_last ) continue ;

            // do not insert spaces inbetween operator combos
            // like +=, -=, <=...
            if( *iter_next == '=' ) 
            { 
                iter = ++s.insert( iter, ' ' ) ;
                iter = s.insert( iter + 2, ' ' ) ; 
                continue ; 
            }

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

natus::ntd::string_t parser::clear_line( natus::ntd::string_rref_t s ) const noexcept
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
        if( p0 != std::string::npos ) s = s.substr( p0 ) ;

        size_t const p1 = s.find_last_not_of( ' ' ) ;
        if( p1 != std::string::npos ) s = s.substr( 0, p1+1 ) ;

        if( s.empty() ) return "" ;
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

//******************************************************************************************************
natus::ntd::string_t parser::remove_comment_lines( natus::ntd::string_rref_t s ) const noexcept
{
    // 1. clear all //
    {
        size_t p0 = s.find( "//" ) ;
        while( p0 != std::string::npos )
        {
            size_t const p1 = s.find_first_of( '\n', p0 ) ;
            s = s.substr( 0, p0 ) + s.substr( p1 ) ;

            p0 = s.find( "//", p0 ) ;
        }
    }

    // 2. clear all /**/
    {
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

//******************************************************************************************************
natus::ntd::vector< natus::ntd::string_t > parser::tokenize( natus::ntd::string_cref_t s ) const noexcept
{
    natus::ntd::vector< natus::ntd::string_t > tokens ;

    size_t off = 0 ;
    size_t pos = s.find_first_of( ' ' ) ;
    while( pos != std::string::npos )
    {
        if( (pos - off) != 0 ) tokens.emplace_back( s.substr( off, pos - off ) ) ;

        off = pos + 1 ;
        pos = s.find_first_of( ' ', off ) ;
    }
    tokens.emplace_back( s.substr( off ) ) ;

    return std::move( tokens ) ;
}

//******************************************************************************************************
natus::nsl::post_parse::used_buildins_t parser::determine_used_buildins( natus::ntd::vector< natus::ntd::string_t > const & lines ) const noexcept 
{
    natus::nsl::post_parse::used_buildins_t ret ;

    for( size_t i=0; i<lines.size(); ++i )
    {
        auto const & line = lines[i] ;

        auto const tokens = this_t::tokenize( line ) ;

        size_t j = 0 ;
        for( auto const & token : tokens )
        {
            auto const bi = natus::nsl::get_build_in_by_opcode( token ) ;
            if( bi.t != natus::nsl::buildin_type::unknown ) 
            {
                ret.emplace_back( natus::nsl::post_parse::used_buildin_t { i, j, bi } ) ;
            }
            ++j ;
        }
    }

    return std::move( ret ) ;
}