#pragma once

#include "typedefs.h"
#include "api.h"
#include "result.h"

#include <natus/core/assert.h>

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API semaphore
        {
            natus_this_typedefs( semaphore ) ;

        public:

            class direct_access
            {
            private:

                this_ref_t sem ;

            public:

                direct_access( this_ref_t s ) : sem( s ) {}
                direct_access( direct_access const& ) = delete ;
                direct_access( direct_access&& ) = delete ;

                direct_access& operator ++( void_t )
                {
                    ++sem ;
                    return *this ;
                }

                direct_access& operator --( void_t )
                {
                    --sem ;
                    return *this ;
                }
            };
            natus_typedef( direct_access ) ;
            friend class direct_acess ;

        private:

            natus::concurrent::mutex_t _mtx ;
            natus::concurrent::condition_variable_t _cv ;

            size_t _count = 0 ;

        public:

            typedef ::std::function< bool_t ( size_t const ) > comp_funk_t ;
            typedef ::std::function< void_t ( direct_access_ref_t ) > access_funk_t ;

        public: // ctors

            semaphore( void_t ) : _count( 0 )
            {}

            semaphore( this_cref_t ) = delete ;

            semaphore( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

            semaphore( size_t c ) : _count( c )
            {}

            ~semaphore( void_t )
            {
                natus_assert( _count == 0 ) ;
            }

        public: // operators

            this_ref_t operator = ( this_rref_t rhv )
            {
                _count = rhv._count ;
                return *this ;
            }

            bool_t operator <= ( size_t c )
            {
                return _count <= c ;
            }

            bool_t operator > ( size_t c )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _count > c ;
            }

            bool_t operator >= ( size_t c )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _count >= c ;
            }

            bool_t operator == ( size_t c )
            {
                return _count == c ;
            }

        private:

            this_ref_t operator ++( void_t )
            {
                ++_count ;
                return *this ;
            }

            this_ref_t operator --( void_t )
            {
                --_count ;
                return *this ;
            }

        public:

            bool_t increment( void_t )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                ++_count ;

                return true ;
            }

            bool_t increment( size_t const max_count )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                bool_t const hit_max = _count < max_count ;
                _count = hit_max ? _count + 1 : _count ;

                return true ;
            }

            bool_t decrement( void_t )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                if( _count == 0 )
                    return false ;

                --_count ;

                if( _count == 0 )
                    _cv.notify_all() ;

                return true ;
            }

            bool_t decrement( comp_funk_t funk )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                if( _count == 0 )
                    return false ;

                --_count ;

                if( _count == 0 )
                    _cv.notify_all() ;

                return funk( _count ) ;
            }

            /// wait until semaphore becomes 0
            void_t wait( void_t )
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                while( _count > 0 ) _cv.wait( lk ) ;
            }

            /// 1. wait until cfunk becomes true
            /// 2. funk is called with locked semaphore => keep it simple and small
            void_t wait( comp_funk_t cfunk, access_funk_t funk )
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                while( natus::core::is_not( cfunk( _count ) ) ) _cv.wait( lk ) ;

                {
                    this_t::direct_access da( *this ) ;
                    funk( da ) ;
                }
            }
        };
        natus_typedef( semaphore ) ;

        namespace semaphore_static
        {
            static const semaphore::comp_funk_t is_zero_funk =
                [=] ( size_t const c ) { return c == 0 ; } ;
            static const semaphore::comp_funk_t is_true_funk =
                [=] ( size_t const c ) { return true ; } ;
            static const semaphore::access_funk_t increment_funk =
                [=] ( semaphore_t::direct_access_ref_t da ) { ++da ; } ; ;
            static const semaphore::access_funk_t decrement_funk =
                [=] ( semaphore_t::direct_access_ref_t da ) { --da ; } ; ;
            static const semaphore::access_funk_t no_op_funk =
                [=] ( semaphore_t::direct_access_ref_t da ) { } ; ;
        }
    }
}