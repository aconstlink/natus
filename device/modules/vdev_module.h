#pragma once

#include "../imodule.h"
#include "../layouts/game_controller.hpp"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace device
    {
        // a system module that spawns virtual devices like game controllers.
        class NATUS_DEVICE_API vdev_module : public imodule
        {
            natus_this_typedefs( vdev_module ) ;
            
            struct data
            {
                bool_t xbox_added = false ;
                bool_t keyboard_added = false ;
                bool_t mouse_added = false ;

                natus::device::game_device_res_t dev ;
            };
            natus::std::vector < data > _games ;

        public:

            vdev_module( void_t ) ;
            vdev_module( this_cref_t ) = delete ;
            vdev_module( this_rref_t ) ;
            virtual ~vdev_module( void_t ) ;

            this_ref_t operator = ( this_rref_t ) ;

        public:

            virtual void_t search( natus::device::imodule::search_funk_t ) ;
            virtual void_t update( void_t ) ;

        public:

            typedef ::std::function< void_t ( natus::device::imapping_res_t ) > mapping_searach_ft ;
            void_t search( this_t::mapping_searach_ft ) ;

            void_t check_devices( natus::device::imodule_res_t ) ;

        private:

            void_t init_controller_1( natus::device::imodule_res_t ) ;
        } ;
        natus_res_typedef( vdev_module ) ;
    }
}