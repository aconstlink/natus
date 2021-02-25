#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/graphics/async.h>
#include <natus/graphics/object/state_object.h>
#include <natus/graphics/object/array_object.h>
#include <natus/graphics/object/render_object.h>
#include <natus/graphics/object/geometry_object.h>
#include <natus/graphics/variable/variable_set.hpp>


#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API line_render_2d
        {
            natus_this_typedefs( line_render_2d ) ;

        private: // data

            struct line
            {
                struct points
                {
                    natus::math::vec2f_t p0 ;
                    natus::math::vec2f_t p1 ;
                };

                union 
                {
                    points p2 ;
                    natus::math::vec4f_t v4 ;
                    natus::math::vec2f_t a2[2] ;
                };
                natus::math::vec4f_t color ;

                line( void_t ) {}
                line( line const & rhv ) 
                {
                    v4 = rhv.v4 ;
                    color = rhv.color ;
                }
                line( line && rhv ) noexcept
                {
                    v4 = std::move( rhv.v4 ) ;
                    color = std::move( rhv.color ) ;
                }
            };
            natus_typedef( line ) ;

            struct layer
            {
                natus::ntd::vector< line_t > lines ;
            };
            natus_typedef( layer ) ;

            natus::ntd::vector< layer_t > _layers ;

            size_t _num_lines = 0 ;

        private: // graphics

            struct per_layer_render_data
            {
                size_t start = 0 ;
                size_t num_elems = 0 ;
            };
            natus::ntd::vector< per_layer_render_data > _render_data ;

            struct vertex
            {
                natus::math::vec2f_t pos ;
            };

            struct the_data
            {
                natus::math::vec4f_t color ;
            };

            natus::ntd::string_t _name ;
            natus::graphics::async_views_t _asyncs ;
            natus::graphics::state_object_res_t _rs ;
            natus::graphics::array_object_res_t _ao ;
            natus::graphics::shader_object_res_t _so ;
            natus::graphics::render_object_res_t _ro ;
            natus::graphics::geometry_object_res_t _go ;

        public:

            line_render_2d( void_t ) ;
            line_render_2d( this_cref_t ) = delete ;
            line_render_2d( this_rref_t ) ;
            ~line_render_2d( void_t ) ;

            void_t init( natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
            void_t release( void_t ) noexcept ;

        public:

            void_t draw( size_t const, natus::math::vec2f_cref_t p0, natus::math::vec2f_cref_t p1, natus::math::vec4f_cref_t color ) noexcept ;

            void_t draw_rect( size_t const, natus::math::vec2f_cref_t p0, natus::math::vec2f_cref_t p1, 
                natus::math::vec2f_cref_t p2, natus::math::vec2f_cref_t p3, natus::math::vec4f_cref_t color ) noexcept ;

            void_t draw_circle( size_t const, size_t const, natus::math::vec2f_cref_t p0, float_t const r, natus::math::vec4f_cref_t color ) noexcept ;

        public:

            // copy all data to the gpu buffer and transmit the data
            void_t prepare_for_rendering( void_t ) noexcept ;
            void_t render( size_t const ) noexcept ;


        private:

            natus_typedefs( natus::ntd::vector< natus::math::vec2f_t >, circle ) ;
            natus::ntd::vector< circle_t > _circle_cache ;
            circle_cref_t lookup_circle_cache( size_t const ) noexcept ;

            void_t add_variable_set( natus::graphics::render_object_ref_t rc ) noexcept ;

            bool_t has_data_for_layer( size_t const l ) const noexcept ;
        };
        natus_res_typedef( line_render_2d ) ;
    }
}