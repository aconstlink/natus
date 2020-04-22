
#pragma once

#include "../api.h"
#include "../result.h"
#include "../protos.h"

#include "id.hpp"
#include "../configuration/render_configuration.h"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API backend
        {
            natus_this_typedefs( backend ) ;

        public:

            virtual id_t prepare( natus::gpu::render_configurations_in_t ) noexcept = 0 ;
            virtual id_t prepare( id_rref_t id, natus::gpu::render_configurations_in_t ) noexcept = 0 ;
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