

#pragma once

#include "api.h"
#include "typedefs.h"
#include "module_registry.hpp"

namespace natus
{
    namespace format
    {
        class NATUS_FORMAT_API global
        {
            natus_this_typedefs( global ) ;

        private: // singleton stuff

            static this_ptr_t _singleton ;
            static this_ptr_t get( void_t ) ;

        private:

            module_registry_res_t _mr ;

        public:

            global( void_t ) ;
            global( this_cref_t ) = delete ;
            global( this_rref_t ) ;
            ~global( void_t ) ;
            static natus::format::module_registry_res_t registry( void_t ) noexcept ;

        private:

            void_t register_default_registries( void_t ) ;
        };
        natus_typedef( global ) ;
    }
}