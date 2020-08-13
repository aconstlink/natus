#pragma once

#include "api.h"
#include "typedefs.h"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace io
    {
        class NATUS_IO_API database
        {

            natus_this_typedefs( database ) ;

        private:

            struct file_record
            {
                natus::io::path_t location ;
                natus::std::string_t extension ;
                // -1 : invalid
                // -2 : external
                uint64_t offset = uint64_t( -1 ) ;
                uint64_t sib = uint64_t( 0 ) ;
            };

            struct file_header
            {
                // encryption method
                // some infos
                // start of file records
                // number of file records
                char_t padding[ 20 ] ;
            };

            struct file
            {
                natus::io::path_t base ;
                natus::std::string_t name ;

                file_header fh ;
                natus::std::vector< file_record > records ;
            };
            natus_typedef( file ) ;

            file_t _f ;

        public:

            enum class encryption
            {
                none
            };

        public:

            database( void_t ) ;
            database( natus::io::path_cref_t base ) ;

            database( this_cref_t ) = delete ;
            database( this_rref_t rhv ) ;

            ~database( void_t ) ;

        public:

            // gives access to some file system structure
            // or to a natus database file.
            //
            // @param base the base path to the resource
            // @param name the resource file/folder name
            //
            // Filesystem: /base/name
            // Natus File: /base/name.natus
            bool_t init( natus::io::path_cref_t base ) ;

            // pack the initialized resource into a natus file
            bool_t pack( this_t::encryption const = this_t::encryption::none ) ;

            // unpack the initialized resource to a file system
            bool_t unpack( void_t ) ;


            // store from memory
            bool_t store( natus::std::string_cref_t location /*, binary data*/ ) ;

            // load to memory

        private:



        };
        natus_res_typedef( database ) ;
    }
}