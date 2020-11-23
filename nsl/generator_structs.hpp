#pragma once

#include "typedefs.h"
#include "enums.hpp"
#include "symbol.hpp"
#include "parser_structs.hpp"

namespace natus
{
    namespace nsl
    {
        struct variable_mapping
        {
            natus::ntd::string_t new_name ;
            natus::ntd::string_t old_name ;
            natus::nsl::binding binding ;
            natus::nsl::flow_qualifier fq ;
            natus::nsl::shader_type st ;
        };
        natus_typedef( variable_mapping ) ;
        natus_typedefs( natus::ntd::vector< variable_mapping_t >, variable_mappings ) ;

        static size_t find_by( variable_mappings_cref_t mappings, natus::ntd::string_cref_t name, natus::nsl::binding binding, natus::nsl::flow_qualifier const fq, natus::nsl::shader_type const st ) noexcept
        {
            auto iter = std::find_if( mappings.begin(), mappings.end(), [&] ( variable_mapping_cref_t v ) 
            { 
                return v.old_name == name && v.binding == binding && v.fq == fq && st == v.st ;
            } ) ;
            return std::distance( mappings.begin(), iter ) ;
        }

        struct generatable
        {
            natus::nsl::symbols_t missing ;
            natus::nsl::post_parse::library_t::variables_t vars ;
            natus::nsl::post_parse::library_t::fragments_t frags ;
            natus::nsl::post_parse::config_t config ;
        };
        natus_typedef( generatable ) ;

        struct generated_code
        {
            // for possible later observation like bindings
            natus::nsl::generatable_t rres ;

            struct variable
            {
                natus::ntd::string_t name ;
                natus::nsl::binding binding ;
                natus::nsl::flow_qualifier fq ;
            };
            natus_typedef( variable ) ;

            struct code
            {
                natus::nsl::api_type api ;
                natus::ntd::string_t shader ;
            };
            natus_typedef( code ) ;

            struct shader
            {
                natus::nsl::shader_type type ;
                natus::ntd::vector< variable > variables ;
                natus::ntd::vector< code > codes ;
            };
            natus_typedef( shader ) ;
            natus_typedefs( natus::ntd::vector< shader_t >, shaders ) ;
            shaders_t shaders ;

            typedef std::function< void_t ( natus::nsl::shader_type, code_cref_t ) > for_each_code_t ;
            void_t sorted_by_api_type( natus::nsl::api_type const t, for_each_code_t funk ) const noexcept
            {
                for( auto const& s : shaders )
                {
                    for( auto const& c : s.codes )
                    {
                        if( c.api == t ) funk( s.type, c ) ;
                    }
                }
            }
        };
        natus_typedef( generated_code ) ;

        struct repl_sym
        {
            natus::ntd::string_t what ;
            std::function< natus::ntd::string_t ( natus::ntd::vector< natus::ntd::string_t > const& ) > repl ;
        } ;
        natus_typedef( repl_sym ) ;
        natus_typedefs( natus::ntd::vector< repl_sym >, repl_syms ) ;

        static natus::ntd::string_t perform_repl( natus::ntd::string_t s, repl_syms_cref_t repls ) noexcept
        {
            for( auto const& repl : repls )
            {
                size_t p0 = s.find( repl.what ) ;
                while( p0 != std::string::npos )
                {
                    if( (s[p0-1] != ' ') || (s[p0+repl.what.size()] != ' ') ) 
                    {
                        p0 = s.find( repl.what, p0 + repl.what.size() ) ;
                        continue ;
                    }

                    natus::ntd::vector< natus::ntd::string_t > args ;

                    size_t level = 0 ;
                    size_t beg = p0 + repl.what.size() + 3 ;
                    for( size_t i = beg; i < s.size(); ++i )
                    {
                        if( level == 0 && s[ i ] == ',' ||
                            level == 0 && s[ i ] == ')' )
                        {
                            args.emplace_back( s.substr( beg, ( i - 1 ) - beg ) ) ;
                            beg = i + 2 ;
                        }

                        if( s[ i ] == '(' ) ++level ;
                        else if( s[ i ] == ')' ) --level ;
                        if( level == size_t( -1 ) ) break ;
                    }
                    p0 = s.replace( p0, ( --beg ) - p0, repl.repl( args ) ).find( repl.what ) ;
                }
            }
            return std::move( s ) ;
        }
    }
}