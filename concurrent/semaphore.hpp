#pragma once

#include "typedefs.h"
#include "api.h"
#include "result.h"

#include <natus/core/assert.h>

namespace natus
{
    namespace concurrent
    {
        class semaphore
        {
            natus_this_typedefs( semaphore ) ;

        private:

            natus::concurrent::mutex_t _mtx ;
            natus::concurrent::condition_variable_t _cv ;

            size_t _count = 0 ;

        public:

            typedef ::std::function< bool_t ( size_t const ) > comp_funk_t ;

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

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                _count = rhv._count ;
                return *this ;
            }

            bool_t operator <= ( size_t c ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _count <= c ;
            }

            bool_t operator > ( size_t c ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _count > c ;
            }

            bool_t operator >= ( size_t c ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _count >= c ;
            }

            bool_t operator == ( size_t c ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _count == c ;
            }

        public:

            this_ref_t operator ++( void_t ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                ++_count ;
                return *this ;
            }

            this_ref_t operator --( void_t ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                --_count ;
                return *this ;
            }

        public:

            bool_t increment( void_t ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                ++_count ;

                return true ;
            }

            bool_t increment( size_t const max_count ) noexcept
            { 
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                bool_t const hit_max = _count < max_count ;
                _count = hit_max ? _count + 1 : _count ;

                return true ;
            }

            bool_t decrement( void_t ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                if( _count == 0 )
                    return false ;

                --_count ;

                if( _count == 0 )
                    _cv.notify_all() ;

                return true ;
            }

            bool_t decrement( comp_funk_t funk ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;

                if( _count == 0 )
                    return false ;

                --_count ;

                if( _count == 0 )
                    _cv.notify_all() ;

                return funk( _count ) ;
            }

            /// wait until semaphore becomes value
            void_t wait( size_t const value = 0, int_t const inc = 0 ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                while( _count != value ) _cv.wait( lk ) ;
                _count += inc ;
            }
        };
        natus_typedef( semaphore ) ;
    }
}