#pragma once

#include "api.h"
#include "typedefs.h"
#include "handle.h"
#include "monitor.hpp"
#include "obfuscator.hpp"

#include <natus/concurrent/isleep.hpp>
#include <natus/concurrent/mrsw.hpp>
#include <natus/ntd/vector.hpp>
#include <natus/ntd/filesystem.hpp>

namespace natus
{
    namespace io
    {
        class NATUS_IO_API database
        {
            natus_this_typedefs( database ) ;

        private: // monitoring stuff

            natus::concurrent::thread_t _update_thread ;
            natus::concurrent::interruptable_sleep_t _isleep ;

        public:

            typedef std::function< void_t ( char_cptr_t, size_t const ) > load_completion_funk_t ;

        private:
            
            // this is the cache of a file record.
            // it holds :
            // - the load/store handles of an operation
            // - the cached data
            // - the owner database
            // - the index into the databases' file record
            struct record_cache ;
            natus_res_typedef( record_cache ) ;
            friend struct record_cache ;

            
            struct file_record
            {
                natus::ntd::string_t location ;
                natus::ntd::string_t extension ;
                // -1 : invalid
                // -2 : external
                // otherwise : stored in .natus db file
                uint64_t offset = uint64_t( -1 ) ;
                uint64_t sib = uint64_t( 0 ) ;

                natus::io::path_t rel ;
                natus::ntd::filesystem::file_time_type stamp ;

                natus::ntd::vector< natus::io::monitor_res_t > monitors ;

                // the handle prototype/origin which is copied to the user
                record_cache_res_t cache ;

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
                    cache = rhv.cache ;
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
                    cache = std::move( rhv.cache ) ;
                }
                ~file_record( void_t ) {}

                file_record & operator = ( file_record const& rhv ) noexcept
                {
                    location = rhv.location ;
                    extension = rhv.extension ;
                    offset = rhv.offset ;
                    sib = rhv.sib ;
                    rel = rhv.rel ;
                    stamp = rhv.stamp ;
                    monitors = rhv.monitors ;
                    cache = rhv.cache;
                    return *this ;
                }

                file_record & operator = ( file_record&& rhv ) noexcept
                {
                    location = ::std::move( rhv.location ) ;
                    extension = ::std::move( rhv.extension ) ;
                    offset = rhv.offset ;
                    sib = rhv.sib ;
                    rel = ::std::move( rhv.rel ) ;
                    stamp = ::std::move( rhv.stamp ) ;
                    monitors = ::std::move( rhv.monitors ) ;
                    cache = std::move( rhv.cache ) ;
                    return *this ;
                }
            };
            natus_typedef( file_record ) ;

        private:

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
                natus::ntd::vector< file_record > records ;
                natus::ntd::vector< natus::io::monitor_res_t > monitors ;
                size_t offset = 0 ;

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
                    offset = rhv.offset ;
                    return *this ;
                }

                db & operator = ( db && rhv )
                {
                    base = ::std::move( rhv.base ) ;
                    records = ::std::move( rhv.records ) ;
                    monitors = ::std::move( rhv.monitors ) ;
                    working = ::std::move( rhv.working ) ;
                    name = ::std::move( rhv.name ) ;
                    offset = rhv.offset ;
                    return *this ;
                }
            };
            natus_typedef( db ) ;

            db_t _db ;
            mutable natus::concurrent::mrsw_t _ac ;

            
            static natus::io::obfuscator_t obfuscator( void_t ) noexcept
            {
                return natus::io::obfuscator_t::variant_1() ;
            }

        public:

            enum class encryption
            {
                none
            };
            
            // user access for file record 
            class NATUS_IO_API cache_access
            {
                natus_this_typedefs( cache_access ) ;

                friend class database ;

            private:

                record_cache_res_t _res ;
                cache_access( record_cache_res_t res ) : _res( std::move( res ) ) {}

            public:

                cache_access( this_cref_t rhv ) noexcept : _res( rhv._res ) {}
                cache_access( this_rref_t rhv ) noexcept : _res( std::move( rhv._res ) ) {}
                ~cache_access( void_t ) noexcept {}
                this_ref_t operator = ( this_cref_t rhv ) noexcept { _res = rhv._res ; return *this ; }
                this_ref_t operator = ( this_rref_t rhv ) noexcept { _res = std::move(rhv._res) ; return *this ; }

                // wait for io operation to be finished or taken from data cache
                // if cache exists, it will be preferred. Data cache should be updated
                // automatically on file change which will be notified by the monitor.
                void_t wait_for_operation( natus::io::database::load_completion_funk_t funk ) ;

                // load the same location.
                this_t load( bool_t const reload = false ) noexcept ;

                // changes the location of this record cache access.
                this_t load( natus::ntd::string_cref_t loc, bool_t const reload = false ) noexcept ;
            };
            natus_res_typedef( cache_access ) ;

        public:

            database( void_t ) ;
            database( natus::io::path_cref_t base, natus::io::path_cref_t working_rel = "./",
                natus::io::path_cref_t name = "db" ) ;

            database( this_cref_t ) = delete ;
            database( this_rref_t rhv ) ;
            ~database( void_t ) ;

        public:

            // gives access to some file system structure
            // or to a natus database file.
            //
            // @param base the base path to the database file
            // @param working_rel where the data should be look for/stored at relative to base
            // @param name the db file/folder name
            //
            // Filesystem: /base/name
            // Natus File: /base/name.natus
            bool_t init( natus::io::path_cref_t base, natus::io::path_cref_t working_rel = "./", 
                natus::io::path_cref_t name = "db" ) ;

            // pack the initialized resource into a natus file
            bool_t pack( this_t::encryption const = this_t::encryption::none ) ;

            // unpack the initialized resource to a file system
            bool_t unpack( void_t ) ;


            // store from memory
            natus::io::store_handle_t store( natus::ntd::string_cref_t location , char_cptr_t, size_t const ) ;
            
            // load to memory
            // @param loc the file location
            // @param reload force reload from database( filesystem/ndb file )
            this_t::cache_access_t load( natus::ntd::string_cref_t loc, bool_t const reload = false ) ;

            void_t dump_to_std( void_t ) const noexcept ;

        public: // monitor

            void_t attach( natus::ntd::string_cref_t, natus::io::monitor_res_t ) noexcept ;
            void_t detach( natus::ntd::string_cref_t, natus::io::monitor_res_t ) noexcept ;
            
            void_t attach( natus::io::monitor_res_t ) noexcept ;
            void_t detach( natus::io::monitor_res_t mon ) noexcept ;

        public:
            
            bool_t lookup_extension( natus::ntd::string_cref_t loc, natus::ntd::string_out_t ) const noexcept ;

        private:

            file_record_t create_file_record( this_t::db_ref_t, natus::io::path_cref_t ) const noexcept;
            bool_t lookup( natus::ntd::string_cref_t loc, file_record_out_t ) const noexcept ;
            bool_t lookup( db const & db_, natus::ntd::string_cref_t loc, file_record_out_t ) const noexcept ;
            void_t file_change_stamp( this_t::file_record_cref_t ) noexcept ;
            void_t file_remove( this_t::file_record_cref_t ) noexcept ;
            
            void_t file_change_external( this_t::file_record_cref_t ) noexcept ;
            static void_t file_change_external( db & db_, this_t::file_record_cref_t ) noexcept ;

            natus::ntd::string_t location_for_index( size_t const ) const ;

        private: // monitor
            
            void_t spawn_update( void_t ) noexcept ;
            void_t join_update( void_t ) noexcept ;

        private:

            void_t load_db_file( db & db_, natus::io::path_cref_t ) ;

        };
        natus_res_typedef( database ) ;
    }
}
