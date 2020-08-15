#pragma once

#include "api.h"
#include "typedefs.h"
#include "handle.h"
#include "monitor.hpp"

#include <natus/concurrent/isleep.hpp>
#include <natus/concurrent/mrsw.hpp>
#include <natus/std/vector.hpp>

namespace natus
{
    namespace io
    {
        class NATUS_IO_API database
        {
            natus_this_typedefs( database ) ;

        private: // monitoring stuff

            natus::concurrent::thread_t _monitor_thread ;
            natus::concurrent::interruptable_sleep_t _isleep ;

        private:

            struct file_record
            {
                natus::std::string_t location ;
                natus::std::string_t extension ;
                // -1 : invalid
                // -2 : external
                // otherwise : stored in .natus db file
                uint64_t offset = uint64_t( -1 ) ;
                uint64_t sib = uint64_t( 0 ) ;

                natus::io::path_t rel ;
                natus::std::filesystem::file_time_type stamp ;

                natus::std::vector< natus::io::monitor_res_t > monitors ;

                file_record( void_t ) {}
                file_record( file_record const & rhv ) 
                {
                    location = rhv.location ;
                    extension = rhv.extension ;
                    offset = rhv.offset ;
                    sib = rhv.sib ;
                    rel = rhv.rel ;
                    stamp = rhv.stamp ;
                    monitors = rhv.monitors ;
                }

                file_record( file_record && rhv )
                {
                    location = ::std::move( rhv.location ) ;
                    extension = ::std::move( rhv.extension ) ;
                    offset = rhv.offset ;
                    sib = rhv.sib ;
                    rel = ::std::move( rhv.rel ) ;
                    stamp = ::std::move( rhv.stamp ) ;
                    monitors = ::std::move( rhv.monitors ) ;
                }
                ~file_record( void_t ) {}

                file_record & operator = ( file_record const& rhv )
                {
                    location = rhv.location ;
                    extension = rhv.extension ;
                    offset = rhv.offset ;
                    sib = rhv.sib ;
                    rel = rhv.rel ;
                    stamp = rhv.stamp ;
                    monitors = rhv.monitors ;
                    return *this ;
                }

                file_record & operator = ( file_record&& rhv )
                {
                    location = ::std::move( rhv.location ) ;
                    extension = ::std::move( rhv.extension ) ;
                    offset = rhv.offset ;
                    sib = rhv.sib ;
                    rel = ::std::move( rhv.rel ) ;
                    stamp = ::std::move( rhv.stamp ) ;
                    monitors = ::std::move( rhv.monitors ) ;
                    return *this ;
                }
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
                natus::io::path_t working ;
                natus::io::path_t name ;
                natus::std::vector< file_record > records ;
                natus::std::vector< natus::io::monitor_res_t > monitors ;

                db( void_t ) {}
                db( db const& rhv ) { *this = rhv ; }
                db( db && rhv ) { *this = ::std::move( rhv ) ; }
                ~db( void_t ) {}

                db & operator = ( db const& rhv )
                {
                    base = rhv.base ;
                    records = rhv.records ;
                    monitors = rhv.monitors ;
                    working = rhv.working ;
                    name = rhv.name ;
                    return *this ;
                }

                db & operator = ( db && rhv )
                {
                    base = ::std::move( rhv.base ) ;
                    records = ::std::move( rhv.records ) ;
                    monitors = ::std::move( rhv.monitors ) ;
                    working = ::std::move( rhv.working ) ;
                    name = ::std::move( rhv.name ) ;
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
            // @param base the base path to the database file
            // @param working_rel where the data should be look for/stored at relative to base
            // @param name the resource file/folder name
            //
            // Filesystem: /base/name
            // Natus File: /base/name.natus
            bool_t init( natus::io::path_cref_t base, natus::io::path_cref_t name = "db", 
                natus::io::path_cref_t working_rel = "./" ) ;

            // pack the initialized resource into a natus file
            bool_t pack( this_t::encryption const = this_t::encryption::none ) ;

            // unpack the initialized resource to a file system
            bool_t unpack( void_t ) ;


            // store from memory
            natus::io::store_handle_t store( natus::std::string_cref_t location , char_cptr_t, size_t const ) ;

            // load to memory
            natus::io::load_handle_t load( natus::std::string_cref_t loc ) const ;

            void_t dump_to_std( void_t ) const noexcept ;

        public: // monitor

            void_t attach( natus::std::string_cref_t, natus::io::monitor_res_t ) noexcept ;
            void_t detach( natus::std::string_cref_t, natus::io::monitor_res_t ) noexcept ;
            
            void_t attach( natus::io::monitor_res_t ) noexcept ;
            void_t detach( natus::io::monitor_res_t mon ) noexcept ;

        private:

            file_record_t create_file_record( natus::io::path_cref_t, natus::io::path_cref_t ) const noexcept;
            bool_t lookup( natus::std::string_cref_t loc, file_record_out_t ) const noexcept ;
            bool_t lookup( db const & db_, natus::std::string_cref_t loc, file_record_out_t ) const noexcept ;
            void_t file_change_stamp( this_t::file_record_cref_t ) noexcept ;
            void_t file_remove( this_t::file_record_cref_t ) noexcept ;

        private: // monitor
            
            void_t spawn_monitor( void_t ) noexcept ;
            void_t join_monitor( void_t ) noexcept ;

        private:

            void_t load_db_file( natus::io::path_cref_t ) ;

        };
        natus_res_typedef( database ) ;
    }
}