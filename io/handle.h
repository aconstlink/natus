#pragma once

#include "api.h"
#include "typedefs.h"
#include "protos.h"

namespace natus
{
    namespace io
    {
        class NATUS_IO_API load_handle
        {
            natus_this_typedefs( load_handle ) ;

            friend class system ;

        private:

            natus::io::internal_item_ptr_t _data_ptr = nullptr ;
            natus::io::system_ptr_t _ios = nullptr ;

        private:

            load_handle( natus::io::internal_item_ptr_t, natus::io::system_ptr_t ) ;

        public:

            load_handle( void_t ) ;
            load_handle( this_rref_t ) ;
            ~load_handle( void_t ) ;

            load_handle( this_cref_t ) = delete ;

        public:

            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t ) ;

        public:

            natus::io::result wait_for_operation( natus::io::load_completion_funk_t ) ;
            bool_t can_wait( void_t ) const noexcept ;
        };
        natus_typedef( load_handle ) ;

        class NATUS_IO_API store_handle
        {
            natus_this_typedefs( store_handle ) ;

            friend class system ;

        private:

            natus::io::internal_item_ptr_t _data_ptr = nullptr ;
            natus::io::system_ptr_t _ios = nullptr ;

        private:

            store_handle( natus::io::internal_item_ptr_t, natus::io::system_ptr_t ) ;

        public:

            store_handle( void_t ) ;
            store_handle( this_rref_t ) ;
            ~store_handle( void_t ) ;

            store_handle( this_cref_t ) = delete ;

        public:

            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t ) ;

        public:

            natus::io::result wait_for_operation( void_t ) ;
            natus::io::result wait_for_operation( natus::io::store_completion_funk_t ) ;
        };
        natus_typedef( store_handle ) ;
    }
}