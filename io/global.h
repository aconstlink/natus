#pragma once

#include "api.h"
#include "protos.h"
#include "typedefs.h"

#include "handle.h"

#include <natus/concurrent/mutex.hpp>

namespace natus
{
    namespace io
    {
        class NATUS_IO_API global
        {
            natus_this_typedefs( global ) ;

        private:

            natus::io::system_ptr_t _ios = nullptr ;

        private:

            static natus::concurrent::mutex_t _mtx ;
            static this_ptr_t _ptr ;

        public:

            global( void_t ) ;
            global( this_cref_t ) = delete ;
            global( this_rref_t ) ;
            ~global( void_t ) ;

        public:

            static this_ptr_t create( this_rref_t, natus::memory::purpose_cref_t ) ;
            static void_t destroy( this_ptr_t ) ;

        public:

            static this_ptr_t init( void_t ) ;
            static void_t deinit( void_t ) ;
            static this_ptr_t get( void_t ) ;
            static system_ptr_t io_system( void_t ) ;

            static natus::io::load_handle_t load( natus::io::path_cref_t ) ;
            static natus::io::load_handle_t load( natus::io::path_cref_t, size_t const offset, size_t const sib ) ;
            static natus::io::store_handle_t store( natus::io::path_cref_t, char_cptr_t, size_t ) ;
        };
        natus_typedef( global ) ;
    }
}