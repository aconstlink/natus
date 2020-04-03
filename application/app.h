
#pragma once

#include "api.h"
#include "typedefs.h"
#include "result.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API app
        {
        public:

            virtual ~app( void_t ) {}

        public:

            virtual natus::application::result init( void_t ) = 0 ;
            virtual natus::application::result update( void_t ) = 0 ;
            virtual natus::application::result visualize( void_t ) = 0 ;
            virtual natus::application::result shutdown( void_t ) = 0 ;
        };
        
    }
}