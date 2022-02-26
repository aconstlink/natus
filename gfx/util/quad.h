
#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/graphics/async.h>
#include <natus/graphics/object/render_object.h>
#include <natus/graphics/object/geometry_object.h>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API quad
        {
            natus_this_typedefs( quad ) ;

        private:

            natus::ntd::string_t _name ;
            
            natus::math::mat4f_t _view = natus::math::mat4f_t().identity() ;
            natus::math::mat4f_t _proj = natus::math::mat4f_t().identity() ;
            natus::math::mat4f_t _world = natus::math::mat4f_t().identity() ;

            natus::graphics::state_object_res_t _rs = natus::graphics::state_object_t() ;
            natus::graphics::render_object_res_t _ro = natus::graphics::render_object_t() ;
            natus::graphics::geometry_object_res_t _go = natus::graphics::geometry_object_t() ;

            struct vertex { natus::math::vec3f_t pos ; } ;

        public:

            quad( natus::ntd::string_cref_t name ) noexcept ;
            quad( this_cref_t ) = delete ;
            quad( this_rref_t ) noexcept ;
            ~quad( void_t ) noexcept ;

        public:
            
            void_t set_view_proj( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) noexcept ;

            void_t set_position( natus::math::vec2f_cref_t ) noexcept ;
            void_t set_position( size_t const vs, natus::math::vec2f_cref_t ) noexcept ;
            void_t set_scale( natus::math::vec2f_cref_t ) noexcept ;
            void_t set_texture( natus::ntd::string_cref_t ) noexcept ;
            void_t set_texture( size_t const i, natus::ntd::string_cref_t ) noexcept ;

            void_t init( natus::graphics::async_views_t, size_t const vs = 1 ) noexcept ;
            void_t release( natus::graphics::async_views_t ) noexcept ;
            void_t render( natus::graphics::async_views_t ) noexcept ;
            void_t render( size_t const, natus::graphics::async_views_t ) noexcept ;

            void_t add_variable_sets( natus::graphics::async_views_t, size_t const vs ) noexcept ;
            size_t get_num_variable_sets( void_t ) const noexcept ;
        };
        natus_res_typedef( quad ) ;
    }
}