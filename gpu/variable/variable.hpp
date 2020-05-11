
#pragma once

#include "../types.h"
#include "../typedefs.h"

namespace natus
{
    namespace gpu
    {
        class ivariable
        {
        public:

            virtual ~ivariable( void_t ) {}
        };
        natus_typedef( ivariable ) ;
        typedef ::std::shared_ptr< natus::gpu::ivariable_ptr_t > ivar_str_t ;

        template< class T >
        class variable : public ivariable
        {
            natus_this_typedefs( variable< T > ) ;

        private:

            T _value ;

            // copy funk

        public:

            variable( void_t ) 
            {}

            variable( this_cref_t ) 
            {}

            variable( this_rref_t )
            {}

            virtual ~variable( void_t ) {}

        public:



        };
    }
}