
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
#include "../object/state_object.h"
#include "../object/array_object.h"
#include "../object/streamout_object.h"
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
                size_t geo = size_t( 0 ) ;
                size_t state_set = size_t( -1 ) ;
                bool_t feed_from_streamout = false ;
            };
            natus_typedef( render_detail ) ;

            enum class pop_type
            {
                render_state
            };

            enum class unuse_type
            {
                framebuffer,
                streamout
            };

        public: // user interface

            virtual natus::graphics::result configure( natus::graphics::geometry_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::render_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::shader_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::image_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::framebuffer_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::state_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::array_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result configure( natus::graphics::streamout_object_res_t ) noexcept = 0 ;

            virtual natus::graphics::result release( natus::graphics::geometry_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result release( natus::graphics::render_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result release( natus::graphics::shader_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result release( natus::graphics::image_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result release( natus::graphics::framebuffer_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result release( natus::graphics::state_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result release( natus::graphics::array_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result release( natus::graphics::streamout_object_res_t ) noexcept = 0 ;

            // @obsolete
            virtual natus::graphics::result connect( natus::graphics::render_object_res_t, natus::graphics::variable_set_res_t ) noexcept = 0 ;

            virtual natus::graphics::result update( natus::graphics::geometry_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result update( natus::graphics::array_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result update( natus::graphics::streamout_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result update( natus::graphics::image_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result update( natus::graphics::render_object_res_t, size_t const varset ) noexcept = 0 ;

            virtual natus::graphics::result use( natus::graphics::framebuffer_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result use( natus::graphics::streamout_object_res_t ) noexcept = 0 ;
            virtual natus::graphics::result unuse( natus::graphics::backend::unuse_type const ) noexcept = 0 ;
            virtual natus::graphics::result push( natus::graphics::state_object_res_t, size_t const, bool_t const ) noexcept = 0 ;
            virtual natus::graphics::result pop( pop_type const ) noexcept = 0 ;

            virtual natus::graphics::result render( natus::graphics::render_object_res_t, natus::graphics::backend::render_detail_cref_t ) noexcept = 0 ;

        public: // engine interface

            virtual void_t render_begin( void_t ) noexcept = 0 ;
            virtual void_t render_end( void_t ) noexcept = 0 ;

        public: // 

            backend( void_t ) noexcept ;
            backend( natus::graphics::backend_type const bt ) ;
            backend( this_rref_t ) noexcept ;
            virtual ~backend( void_t ) noexcept ;

            natus::graphics::backend_type get_type( void_t ) const noexcept { return _bt ; }

            static natus::graphics::render_state_sets_t default_render_states( void_t )
            {
                return natus::graphics::render_state_sets_t() ;
            }

        protected:

            size_t get_bid( void_t ) const { return size_t( _bt ) ; }
            void_t set_type( natus::graphics::backend_type const bt ) { _bt = bt ; }

        private:
            
            static size_t create_backend_id( void_t ) noexcept ;

        };
        natus_res_typedef( backend ) ;
    }
}