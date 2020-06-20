
#pragma once

#include "../api.h"
#include "../result.h"
#include "../protos.h"


#include "../id.hpp"
#include "../shader/shader_configuration.h"
#include "../configuration/render_configuration.h"
#include "../configuration/geometry_configuration.h"
#include "../variable/variable_set.hpp"

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

            natus::gpu::backend_type _bt = backend_type::unknown ;

            struct render_detail
            {
                size_t start = size_t( 0 ) ;
                size_t num_elems = size_t( -1 ) ;
                size_t varset = size_t( 0 ) ;
            };
            natus_typedef( render_detail ) ;

        public:

            virtual id_t configure( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept = 0 ;
            virtual natus::gpu::result configure( natus::gpu::render_configuration_res_t ) noexcept = 0 ;
            virtual id_t configure( id_rref_t id, natus::gpu::shader_configuration_res_t ) noexcept = 0 ;

            virtual id_t connect( id_rref_t id, natus::gpu::variable_set_res_t ) noexcept = 0 ;
            virtual id_t update( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept = 0 ;

            virtual natus::gpu::result render( natus::gpu::render_configuration_res_t, natus::gpu::backend::render_detail_cref_t ) noexcept = 0 ;

            //virtual id_t feedback( id_rref_t id ) noexcept = 0 ;
            //virtual id_t compute( id_rref_t id ) noexcept = 0 ;

            virtual void_t render_begin( void_t ) noexcept = 0 ;
            virtual void_t render_end( void_t ) noexcept = 0 ;

        public: // 

            backend( void_t ) noexcept ;
            backend( natus::gpu::backend_type const bt ) ;
            backend( this_rref_t ) noexcept ;
            virtual ~backend( void_t ) noexcept ;

            natus::gpu::backend_type get_type( void_t ) const noexcept { return _bt ; }

        protected:

            size_t get_bid( void_t ) const { return _backend_id ; }
            void_t set_type( natus::gpu::backend_type const bt ) { _bt = bt ; }

        private:

            size_t _backend_id = size_t(-1) ;
            static size_t create_backend_id( void_t ) noexcept ;

        };
        natus_soil_typedef( backend ) ;
    }
}