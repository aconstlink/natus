#pragma once

#include "../typedefs.h"
#include "../mutex.hpp"

namespace natus
{
    namespace concurrent
    {
        template< typename T >
        class vector
        {
            natus_this_typedefs( vector<T> ) ;

            natus_typedefs( T, type ) ;
            natus_typedefs( natus::ntd::vector<T>, container ) ;

        private:

            mutable natus::concurrent::mutex_t _mtx ;
            container_t _cont ;

        public:

            vector( void_t )
            {}

            vector( this_rref_t rhv )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                _cont = ::std::move( rhv._cont ) ;
            }

            vector( this_cref_t rhv )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                _cont = rhv._cont ;
            }

            ~vector( void_t )
            {}

        public:

            this_ref_t push_back( type_cref_t v )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                _cont.push_back( v ) ;

                return *this ;
            }

            size_t size( void_t ) const
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return _cont.size() ;
            }

            container_t move_out( void_t )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                return ::std::move( _cont ) ;
            }
        };
    }
}