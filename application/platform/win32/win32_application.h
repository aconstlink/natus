#pragma once

#include "../../api.h"
#include "../platform_application.h"
#include <natus/device/modules/win32/rawinput_module.h>

namespace natus
{
    namespace application
    {
        namespace win32
        {
            class NATUS_APPLICATION_API win32_application : public platform_application
            {
                typedef platform_application base_t ;
                natus_this_typedefs( win32_application ) ;

            private:

                /// can be used for special escape key pattern
                size_t _escape_sequence = 0 ;
                
                natus::device::win32::rawinput_module_res_t _rawinput ;

            public:

                win32_application( void_t ) ;
                win32_application( natus::application::app_res_t ) ;
                win32_application( this_rref_t ) ;
                virtual ~win32_application( void_t ) ;

            public:

                static this_ptr_t create( void_t ) ;
                static this_ptr_t create( this_rref_t ) ;
                static void_t destroy( this_ptr_t ) ;

            public:

                virtual natus::application::result on_exec( void_t ) ;
            };
            natus_typedef( win32_application ) ;
            typedef natus::soil::res< win32_application > application_res_t ;
        }
    }
}