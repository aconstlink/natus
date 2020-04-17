
#pragma once

#include "../id.hpp"

#include <natus/soil/rptr.hpp>
#include <natus/concurrent/mutex.hpp>

namespace natus
{
    namespace gpu
    {
        class completion
        {
            natus_this_typedefs( completion ) ;

        public:

            typedef ::std::function< natus::gpu::id_t ( natus::gpu::id_rref_t, natus::gpu::result ) >
                completion_funk_t ;
            
        private:

            natus::concurrent::mutex_t _mtx ;
            completion_funk_t _funk ;
        
        public:

            completion( void_t ) {}

            completion( completion_funk_t funk ) : _funk( funk ) {}

            completion( this_cref_t ) = delete ;

            completion( this_rref_t rhv ) 
            {
                _funk = ::std::move( rhv._funk ) ; 
            }

            ~completion( void_t ) {}

        public:

            /// allows to be called upon completion of backend operations
            natus::gpu::id_t call_completion( natus::gpu::id_rref_t id, natus::gpu::result res )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _funk( ::std::move( id ), res ) ;
            }

            void_t set( completion_funk_t funk )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                _funk = funk ;
            }

            void_t remove_completion_funk( void_t )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                _funk = [=] ( natus::gpu::id_rref_t id, natus::gpu::result res ) 
                { 
                    return ::std::move( id ) ;  
                } ;
            }
        };
        natus_typedef( completion ) ;
        typedef natus::soil::rptr< completion_ptr_t > completion_rptr_t ;
    }
}