#pragma once

#include "protos.h"
#include "typedefs.h"
#include "result.h"
#include "api.h"

namespace natus
{
    namespace application
    {
        /// rserved for later use
        class /*NATUS_APPLICATION_API*/ system
        {
        private:

            static system_ptr_t _system_ptr ;

        private: // made private

            /// initializes the application layer system for providing application
            /// layer internal singletons.
            static natus::application::result init( void_t ) ;
            static natus::application::result deinit( void_t ) ;

        };
    }
}