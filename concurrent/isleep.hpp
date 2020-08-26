#pragma once

#include "typedefs.h"
#include <chrono>

namespace natus
{
    namespace concurrent
    {
        class interruptable_sleep
        {
            natus_this_typedefs( interruptable_sleep ) ;

        public:

            typedef ::std::chrono::high_resolution_clock clock_t ;

        private:

            natus::concurrent::mutex_t _mtx ;
            natus::concurrent::condition_variable_t _cv ;
            bool_t _interruped = false ;

        public:

            interruptable_sleep( void_t ) {}
            interruptable_sleep( this_cref_t ) = delete ;
            interruptable_sleep( this_rref_t rhv ) { _interruped = rhv._interruped ; }
            ~interruptable_sleep( void_t ) {}

            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t rhv ) { _interruped = rhv._interruped ; return *this ; }

        public:

            // naps for some duration or is interrupted
            // returns the interrupted state
            bool_t sleep_for( clock_t::duration const dur ) noexcept
            {
                natus::concurrent::ulock_t lk( _mtx ) ;
                if( !_interruped ) _cv.wait_until( lk, clock_t::now() + dur ) ;
                
                bool_t interrupted = _interruped ;
                _interruped = false ;
                return interrupted ;
            }

            // naps until a time point or is interrupted
            // returns the interrupted state
            bool_t sleep_until( clock_t::time_point const tp ) noexcept
            {
                natus::concurrent::ulock_t lk( _mtx ) ;
                if( !_interruped ) _cv.wait_until( lk, tp ) ;

                bool_t interrupted = _interruped ;
                _interruped = false ;
                return interrupted ;
            }

            // interrupt nap
            void_t interrupt( void_t ) noexcept
            {
                {
                    natus::concurrent::lock_guard_t lk( _mtx ) ;
                    _interruped = true ;
                }
                _cv.notify_one() ;
            }

            // wakeup from nap
            void_t wakeup( void_t ) noexcept
            {
                _cv.notify_one() ;
            }

            void_t reset( void_t ) noexcept 
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                _interruped = false ;
            }
        };
        natus_typedef( interruptable_sleep ) ;
        natus_typedefs( interruptable_sleep, isleep ) ;
    }
}