
#pragma once

#include "../api.h"
#include "../result.h"
#include "../protos.h"


#include "../id.hpp"
#include "../object/shader_object.h"
#include "../object/render_object.h"
#include "../object/geometry_object.h"
#include "../object/image_object.h"
#include "../object/framebuffer_object.h"
#include "../variable/variable_set.hpp"
#include "../state/state_set.h"

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API backend
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

            natus::graphics::backend_type _bt = backend_type::unknown ;

            struct render_detail
            {
                size_t start = size_t( 0 ) ;
                size_t num_elems = size_t( -1 ) ;
                size_t varset = size_t( 0 ) ;
                natus::graphics::render_state_sets_res_t render_states = 
                    this_t::default_render_states() ;
            };
            natus_typedef( render_detail ) ;

        public:

            virtual natus::graphics::result configure( natus::graphics::geometry_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::render_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::shader_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::image_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::framebuffer_object_res_t ) noexcept = 0 ;

            // @obsolete
            virtual natus::graphics::result connect( natus::graphics::render_object_res_t, natus::graphics::variable_set_res_t ) noexcept = 0 ;

            virtual natus::graphics::result update( natus::graphics::geometry_object_res_t ) noexcept = 0 ;

            virtual natus::graphics::result use( natus::graphics::framebuffer_object_res_t ) noexcept = 0 ;

            virtual natus::graphics::result render( natus::graphics::render_object_res_t, natus::graphics::backend::render_detail_cref_t ) noexcept = 0 ;

            //virtual id_t feedback( id_rref_t id ) noexcept = 0 ;
            //virtual id_t compute( id_rref_t id ) noexcept = 0 ;

            virtual void_t render_begin( void_t ) noexcept = 0 ;
            virtual void_t render_end( void_t ) noexcept = 0 ;

        public: // 

            backend( void_t ) noexcept ;
            backend( natus::graphics::backend_type const bt ) ;
            backend( this_rref_t ) noexcept ;
            virtual ~backend( void_t ) noexcept ;

            natus::graphics::backend_type get_type( void_t ) const noexcept { return _bt ; }

            static natus::graphics::render_state_sets_res_t default_render_states( void_t )
            {
                static natus::graphics::render_state_sets_res_t __default_rs = natus::graphics::render_state_sets_t() ;
                return __default_rs ;
            }

        protected:

            size_t get_bid( void_t ) const { return _backend_id ; }
            void_t set_type( natus::graphics::backend_type const bt ) { _bt = bt ; }

        private:

            size_t _backend_id = size_t(-1) ;
            static size_t create_backend_id( void_t ) noexcept ;

        };
        natus_res_typedef( backend ) ;
    }
}