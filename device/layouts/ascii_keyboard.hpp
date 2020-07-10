
#pragma once

#include "../typedefs.h"

namespace natus
{
    namespace device
    {
        namespace layouts
        {
            class ascii
            {
                natus_this_typedefs( ascii ) ;
                natus_typedefs( natus::device::device< this_t >, this_device ) ;

                friend class this_device_t ;

            public:

                enum class input_component
                {
                    num_components
                };

                enum class output_component
                {
                    num_components
                };

            private:

                static void_t init_components( this_t::this_device_ref_t dev )
                {
                    // inputs
                    /*_inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::button_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::button_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::button_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::scroll_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::point_t() ) ) ;
                    */
                    // outputs
                }
            };
            natus_typedef( ascii ) ;
        }
        natus_soil_typedefs( natus::device::device< natus::device::layouts::ascii_t>, ascii_device ) ;
    }
}