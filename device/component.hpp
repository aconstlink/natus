
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
            virtual ~icomponent( void_t ) {}
        };
        natus_typedef( icomponent ) ;

        class input_component : public icomponent
        {
        public:

            virtual ~input_component( void_t ) {}
        };
        natus_typedef( input_component ) ;

        class output_component : public icomponent
        {
        public:
            
            virtual ~output_component( void_t ) {}
        };
        natus_typedef( output_component ) ;
    }
}