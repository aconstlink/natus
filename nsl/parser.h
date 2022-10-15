

#pragma once

#include "api.h"
#include "typedefs.h"
#include "parser_structs.hpp"
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        class NATUS_NSL_API parser
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

            natus::nsl::post_parse::document_t process( natus::ntd::string_cref_t file ) noexcept ;

            // produces a statement list of all statements in the file
            natus::nsl::post_parse::document_t process( natus::ntd::string_rref_t file ) noexcept ;

        private: // 

            natus::nsl::parse::configs_t filter_config_statements( this_t::statements_rref_t ss ) const noexcept ;
            
            natus::nsl::post_parse::configs_t analyse_configs( natus::nsl::parse::configs_rref_t configs ) const noexcept ;

            natus::nsl::parse::libraries_t filter_library_statements( this_t::statements_rref_t ss ) const noexcept ;

            statements_t replace_numbers( statements_rref_t ss ) const ;

            statements_t replace_operators( statements_rref_t ss ) const ;
            statements_t replace_buildins( statements_rref_t ss ) const ;

            natus::nsl::post_parse::libraries_t analyse_libraries( natus::nsl::parse::libraries_rref_t libs ) const noexcept ;

            statements_t filter_for_group( natus::ntd::string_cref_t what, statements_rref_t ss ) const noexcept ;

            // replacing the open/close tags in the statements list makes
            // later processing much easier/less code.
            statements_t replace_open_close( statements_rref_t ss ) const noexcept ;

            statements_t repackage( statements_rref_t ss ) const noexcept ;

        private: //

            bool_t some_first_checks( natus::ntd::string_cref_t file ) const noexcept  ;

            bool_t check_open_close( natus::ntd::string_cref_t open, natus::ntd::string_cref_t close, natus::ntd::string_cref_t file ) const noexcept ;

        private: // scan file

            statements_t scan( natus::ntd::string_rref_t file ) const noexcept ;

        private: // helper

            // ensures that certain characters have spaces in front or behind.
            // this makes later code analysis much easier and just less character checking.
            natus::ntd::string_t insert_spaces( natus::ntd::string_rref_t s ) const noexcept ;

            natus::ntd::string_t clear_line( natus::ntd::string_rref_t s ) const noexcept ;

            // simply removes all comments with
            // // and /**/
            natus::ntd::string_t remove_comment_lines( natus::ntd::string_rref_t s ) const noexcept ;

            natus::ntd::vector< natus::ntd::string_t > tokenize( natus::ntd::string_cref_t s ) const noexcept ;

            natus::nsl::post_parse::used_buildins_t determine_used_buildins( natus::ntd::vector< natus::ntd::string_t > const & ) const noexcept ;
            
        };
        natus_res_typedef( parser ) ;

    }
}