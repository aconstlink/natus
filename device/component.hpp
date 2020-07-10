
#pragma once

#include "api.h"
#include "typedefs.h"

namespace natus
{
    namespace device
    {
        class icomponent
        {
        public: 
            virtual void_t update( void_t ) noexcept = 0 ;
        };
        natus_typedef( icomponent ) ;

        class input_component : public icomponent
        {
        public:

            virtual ~input_component( void_t ) {}
            virtual void_t update( void_t ) noexcept {}
        };
        natus_typedef( input_component ) ;

        class output_component : public icomponent
        {
        public:
            
            virtual ~output_component( void_t ) {}
            virtual void_t update( void_t ) noexcept {}
        };
        natus_typedef( output_component ) ;
    }
}