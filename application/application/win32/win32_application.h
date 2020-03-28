#pragma once

#include "../../api.h"
#include "../window_application.h"

//#include <natus/device/protos.h>

namespace natus
{
    namespace application
    {
        namespace win32
        {
            class NATUS_APPLICATION_API win32_application : public window_application
            {
                typedef window_application base_t ;
                natus_this_typedefs( win32_application ) ;

            private:

                /// can be used for special escape key pattern
                size_t _escape_sequence = 0 ;

                //natus_device::natus_win32::rawinput_api_ptr_t _rawinput_ptr = nullptr ;

            public:

                win32_application( void_t ) ;
                win32_application( this_rref_t ) ;
                virtual ~win32_application( void_t ) ;

            public:

                static this_ptr_t create( void_t ) ;
                static this_ptr_t create( this_rref_t ) ;
                static void_t destroy( this_ptr_t ) ;

            public:

                virtual natus::application::result destroy( void_t ) ;
                virtual natus::application::result exec_derived( void_t ) ;
            };
        }
    }
}