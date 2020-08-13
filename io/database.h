#pragma once

#include "api.h"
#include "typedefs.h"
#include "handle.h"

#include <natus/concurrent/mrsw.hpp>
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
                natus::std::string_t location ;
                natus::std::string_t extension ;
                // -1 : invalid
                // -2 : external
                uint64_t offset = uint64_t( -1 ) ;
                uint64_t sib = uint64_t( 0 ) ;

                natus::io::path_t rel ;
                natus::std::filesystem::file_time_type stamp ;
            };
            natus_typedef( file_record ) ;

            struct file_header
            {
                // encryption method
                // some infos
                // start of file records
                // number of file records
                char_t padding[ 20 ] ;
            };

            struct db
            {
                natus::io::path_t base ;
                natus::std::vector< file_record > records ;

                db( void_t ) {}
                db( db const& rhv ) 
                { 
                    base = rhv.base ;
                    records = rhv.records ;
                }
                db( db && rhv )
                {
                    base = ::std::move( rhv.base ) ;
                    records = ::std::move( rhv.records ) ;
                }
                ~db( void_t ) {}

                db & operator = ( db const& rhv )
                {
                    base = rhv.base ;
                    records = rhv.records ;
                    return *this ;
                }

                db& operator = ( db && rhv )
                {
                    base = ::std::move( rhv.base ) ;
                    records = ::std::move( rhv.records ) ;
                    return *this ;
                }
            };
            natus_typedef( db ) ;

            db_t _db ;
            mutable natus::concurrent::mrsw_t _ac ;

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
            natus::io::store_handle_t store( natus::std::string_cref_t location , char_cptr_t, size_t const ) ;

            // load to memory
            natus::io::load_handle_t load( natus::std::string_cref_t loc ) const ;

            void_t dump_to_std( void_t ) const noexcept ;

        private:

            file_record_t create_file_record( natus::io::path_cref_t, natus::io::path_cref_t ) const noexcept;
            bool_t lookup( natus::std::string_cref_t loc, file_record_out_t ) const noexcept ;
            bool_t lookup( db const & db_, natus::std::string_cref_t loc, file_record_out_t ) const noexcept ;

        };
        natus_res_typedef( database ) ;
    }
}