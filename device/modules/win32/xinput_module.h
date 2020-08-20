
#pragma once

#include "../../imodule.h"
#include "../../layouts/xbox_controller.hpp"

#include <natus/concurrent/mutex.hpp>
#include <natus/ntd/vector.hpp>
#include <natus/math/vector/vector2.hpp>

#include <windows.h>

namespace natus
{
    namespace device
    {
        namespace win32
        {
            class NATUS_DEVICE_API xinput_module : public imodule
            {
                natus_this_typedefs( xinput_module ) ;

                /// helper device for state tracking
                class xinput_device ;
                
                struct gamepad_data
                {
                    bool_t connected = false ;
                    this_t::xinput_device * xinput_ptr = nullptr ;
                    natus::device::xbc_device_res_t dev ;
                };
                natus_typedef( gamepad_data ) ;
                natus_typedefs( natus::ntd::vector<gamepad_data_t>, devices ) ;

            private:

                devices_t _devices ;

            public:

                xinput_module( void_t ) ;
                xinput_module( this_cref_t ) = delete ;
                xinput_module( this_rref_t ) ;
                virtual ~xinput_module( void_t ) ;

                this_ref_t operator = ( this_rref_t ) ;

            public:

                virtual void_t search( natus::device::imodule::search_funk_t ) ;
                virtual void_t update( void_t ) ;

            private:

                void_t check_gamepads( void_t ) ;
            };
            natus_res_typedef( xinput_module ) ;
        }
    }
}