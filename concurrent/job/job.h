#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../result.h"

#include "job_funk.h"

#include <natus/memory/arena/exp_arena.hpp>

namespace natus
{
    namespace concurrent
    {
        class ijob
        {
        public:

            virtual void_t execute( void_t ) = 0 ;
        };
        natus_typedef( ijob ) ;

        template< typename T >
        class job_1d : public ijob
        {
            natus_this_typedefs( job_1d<T> ) ;
            natus_typedefs( T, type ) ;

            natus_typedefs( natus::concurrent::range_1d<T>, this_range ) ;
            natus_typedefs( natus::concurrent::funk_t<T>, this_funk ) ;

        public:

            typedef ::std::function< void_t ( this_ptr_t ) > finish_callback_t ;
            natus_typedefs( natus::memory::experimental::arena< this_t >, this_arena ) ;

        private:

            this_range_t _range ;
            this_funk_t _funk ;

        public:

            job_1d( void_t ) {}
            job_1d( this_range_cref_t r, this_funk_t j ) :
                _range( r ), _funk( j ) {}
            job_1d( this_cref_t rhv ) : _range( rhv._range ), _funk( rhv._funk ) {}
            job_1d( this_rref_t rhv ) : _range( ::std::move( rhv._range ) ), _funk( ::std::move( rhv._funk ) ) {}

            ~job_1d( void_t ) {}

        public:

            virtual void_t execute( void_t )
            {
                _funk( _range ) ;
            }
        };
    }
}
