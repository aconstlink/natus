#pragma once

#include "handle.h"
#include "obfuscator.hpp"

#include <natus/concurrent/typedefs.h>
#include <natus/concurrent/mutex.hpp>
#include <natus/ntd/stack.hpp>

#include <natus/core/macros/move.h>

namespace natus
{
    namespace io
    {
        class NATUS_IO_API system
        {
            natus_this_typedefs( system ) ;

            friend class load_handle ;
            friend class store_handle ;

        private:

            struct load_item
            {
                natus_this_typedefs( load_item ) ;

                char_ptr_t data = nullptr ;
                size_t sib = 0 ;

                ::std::condition_variable cv ;
                natus::concurrent::mutex_t mtx ;
                bool_t ready = false ;
                natus::io::result status = natus::io::result::invalid ;

                load_item( void_t ) {}
                load_item( this_rref_t rhv )
                {
                    natus_move_member_ptr( data, rhv ) ;
                    sib = rhv.sib ;
                    ready = rhv.ready ;
                    status = rhv.status ;
                }
                load_item( this_cref_t ) = delete ;
                ~load_item( void_t ) {}

                this_ref_t operator = ( this_cref_t ) = delete ;
                this_ref_t operator = ( this_rref_t rhv )
                {
                    natus_move_member_ptr( data, rhv ) ;
                    sib = rhv.sib ;
                    ready = rhv.ready ;
                    status = rhv.status ;
                    return *this ;
                }
            };
            natus_typedef( load_item ) ;

            typedef natus::ntd::stack< load_item_ptr_t, 20 > __load_stack_t ;
            natus_typedefs( __load_stack_t, load_stack ) ;
            natus_typedefs( natus::ntd::vector< load_item_ptr_t >, load_items ) ;

            struct store_item
            {
                natus_this_typedefs( store_item ) ;

                char_cptr_t data = nullptr ;
                size_t sib = 0 ;

                ::std::condition_variable cv ;
                natus::concurrent::mutex_t mtx ;
                bool_t ready = false ;
                natus::io::result status = natus::io::result::invalid ;

                store_item( void_t ) {}
                store_item( this_rref_t rhv )
                {
                    natus_move_member_ptr( data, rhv ) ;
                    sib = rhv.sib ;
                    ready = rhv.ready ;
                    status = rhv.status ;
                }
                store_item( this_cref_t ) = delete ;
                ~store_item( void_t ) {}

                this_ref_t operator = ( this_cref_t ) = delete ;
                this_ref_t operator = ( this_rref_t rhv )
                {
                    natus_move_member_ptr( data, rhv ) ;
                    sib = rhv.sib ;
                    ready = rhv.ready ;
                    status = rhv.status ;
                    return *this ;
                }
            };
            natus_typedef( store_item ) ;

            typedef natus::ntd::stack< store_item_ptr_t, 20 > __store_stack_t ;
            natus_typedefs( __store_stack_t, store_stack ) ;
            natus_typedefs( natus::ntd::vector< store_item_ptr_t >, store_items ) ;


        private:

            natus::concurrent::mutex_t _load_mtx ;
            load_stack_t _load_stack ;

            natus::concurrent::mutex_t _store_mtx ;
            store_stack_t _store_stack ;

        public:

            system( void_t ) ;
            ~system( void_t ) ;

            system( this_cref_t ) = delete ;
            system( this_rref_t ) ;

        public:

            static this_ptr_t create( void_t ) ;
            static this_ptr_t create( this_rref_t, natus::memory::purpose_cref_t ) ;
            static void_t destroy( this_ptr_t ) ;

        public:

            natus::io::load_handle_t load( natus::io::path_cref_t, natus::io::obfuscator_rref_t obf,
                size_t const offset = size_t( 0 ), size_t const sib = size_t( -1 ) ) ;            

            natus::io::store_handle_t store( natus::io::path_cref_t, char_cptr_t, size_t const ) ;

        private:

            natus::io::result wait_for_operation( natus::io::load_handle_rref_t, natus::io::load_completion_funk_t ) ;
            natus::io::result wait_for_operation( natus::io::store_handle_rref_t, natus::io::store_completion_funk_t ) ;

        private:

            load_item_ptr_t get_load_item( void_t ) ;
            store_item_ptr_t get_store_item( void_t ) ;

        };
        natus_typedef( system ) ;
    }
}