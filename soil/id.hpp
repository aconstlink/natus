#pragma once

#include "typedefs.h"

#include <natus/memory/global.h>

namespace natus
{
    namespace soil
    {
        class id
        {
            struct ishared_data
            {
                virtual ~ishared_data( void_t ) {}
            };

            template< class T >
            struct shared_data : public ishared_data
            {
                T* ptr = nullptr ;
            };

        public:

            template< class T >
            id( T const & t )
            {
                auto * ptr = natus::memory::global_t::alloc< T >
                    ( t, "[] : T through id" ) ;

                auto* sptr = natus::memory::global_t::alloc< T >
                    ( shared_data<T>(), "[] : T through id" ) ;

                sptr->ptr = ptr ;
            }
        };
        natus_typedef( id ) ;
    }
}