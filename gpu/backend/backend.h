
#pragma once

#include "../api.h"
#include "../result.h"
#include "../protos.h"

#include "id.hpp"
#include "../configuration/render_configuration.h"
#include "../configuration/geometry_configuration.h"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API backend
        {
            natus_this_typedefs( backend ) ;

        public:

            struct window_info
            {
                size_t width = 0 ;
                size_t height = 0 ;
            };
            natus_typedef( window_info ) ;
            virtual void_t set_window_info( window_info_cref_t ) noexcept = 0 ;

        public:

            virtual id_t configure( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept = 0 ;
            virtual id_t configure( id_rref_t id, natus::gpu::render_configurations_res_t ) noexcept = 0 ;

            virtual id_t render( id_rref_t id ) noexcept = 0 ;

            //virtual id_t feedback( id_rref_t id ) noexcept = 0 ;
            //virtual id_t compute( id_rref_t id ) noexcept = 0 ;

        public: // 

            backend( void_t ) noexcept ;
            backend( this_rref_t ) noexcept ;
            virtual ~backend( void_t ) noexcept ;


        protected:

            size_t get_bid( void_t ) const { return _backend_id ; }

        private:

            size_t _backend_id = size_t(-1) ;
            static size_t create_backend_id( void_t ) noexcept ;

        };
        natus_soil_typedef( backend ) ;
    }
}