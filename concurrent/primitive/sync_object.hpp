#pragma once

#include "../typedefs.h"
#include "../protos.h"

namespace natus
{
    namespace concurrent
    {
        /// allows a thread to wait for an object
    /// of this type. The waiting thread will 
    /// go to sleep until the object is signaled.
        class sync_object
        {
            natus_this_typedefs( sync_object ) ;

        private:

            mutable mutex_t _mtx ;
            condition_variable_t _cond ;
            bool_t _condition = false ;

        private:

            sync_object( this_cref_t ) {}

        public:

            sync_object( void_t ) {}
            sync_object( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

            ~sync_object( void_t )
            {
                this_t::set_and_signal() ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _condition = rhv._condition ;
                return *this ;
            }

        public:

            static void_t set_and_signal( this_ptr_t ptr )
            {
                if( natus::core::is_not_nullptr( ptr ) )
                    ptr->set_and_signal() ;
            }

        public:

            /// not locking
            bool_t is_signaled( void_t ) const
            {
                return _condition ;
            }

            /// not locking
            bool_t is_not_signaled( void_t ) const
            {
                return !_condition ;
            }

            void_t set_and_signal( void_t )
            {
                {
                    lock_t lk( _mtx ) ;
                    _condition = true ;
                }
                _cond.notify_all() ;
            }

            /// wait until condition == true
            void_t wait( void_t )
            {
                lock_t lk( _mtx ) ;
                while( !_condition ) _cond.wait( lk ) ;
            }
        } ;
        natus_typedef( sync_object ) ;
    }
}