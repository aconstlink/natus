
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
        class NATUS_GFX_API line_render_3d
        {
            natus_this_typedefs( line_render_3d ) ;

        private:

            struct line
            {
                typedef struct
                {
                    natus::math::vec3f_t p0 ;
                    natus::math::vec3f_t p1 ;
                } points_t ;

                union
                {
                    points_t points ;
                    natus::math::vec3f_t pa[2] ;
                };
                
                natus::math::vec4f_t color ;

                line( void_t ) {}
                line( line const & rhv ) 
                {
                    points = rhv.points ;
                    color = rhv.color ;
                }
                line( line && rhv ) noexcept
                {
                    points = std::move( rhv.points ) ;
                    color = std::move( rhv.color ) ;
                }

                line & operator = ( line const & rhv ) noexcept
                {
                    points = rhv.points ;
                    color = rhv.color ;
                    return *this ;
                }
                line & operator = ( line && rhv ) noexcept
                {
                    points = std::move( rhv.points ) ;
                    color = std::move( rhv.color ) ;
                    return *this ;
                }
            };
            natus_typedef( line ) ;

            struct vertex
            {
                natus::math::vec3f_t pos ;
            };

            struct the_data
            {
                natus::math::vec4f_t color ;
            };

        private:

            natus::ntd::string_t _name ;
            natus::graphics::async_views_t _asyncs ;
            natus::graphics::state_object_res_t _rs ;
            natus::graphics::array_object_res_t _ao ;
            natus::graphics::shader_object_res_t _so ;
            natus::graphics::render_object_res_t _ro ;
            natus::graphics::geometry_object_res_t _go ;

            natus::math::mat4f_t _proj ;
            natus::math::mat4f_t _view ;

            natus::concurrent::mutex_t _lines_mtx ;
            natus::ntd::vector< line_t > _lines ;
            size_t _num_lines = 0 ;

        public:

            line_render_3d( void_t ) noexcept ;
            line_render_3d( this_cref_t ) = delete ;
            line_render_3d( this_rref_t ) noexcept ;
            ~line_render_3d( void_t ) noexcept ;

        public:

            void_t init( natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
            void_t release( void_t ) noexcept ;

        public: // draw functions

            void_t draw( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec4f_cref_t color ) noexcept ;


            #if 0 // for later
            void_t draw_rect( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, 
                natus::math::vec3f_cref_t p2, natus::math::vec3f_cref_t p3, natus::math::vec4f_cref_t color ) noexcept ;

            void_t draw_cube( natus::math::vec3f_cref_t center, float_t const half, natus::math::vec4f_cref_t color ) noexcept ;

            void_t draw_circle( natus::math::vec2f_cref_t center, float_t const r, size_t const num_points, natus::math::vec4f_cref_t color ) noexcept ;
            #endif

        public:

            // copy all data to the gpu buffer and transmit the data
            void_t prepare_for_rendering( void_t ) noexcept ;
            void_t render( void_t ) noexcept ;

            void_t set_view_proj( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t ) noexcept ;

        private:

            void_t add_variable_set( natus::graphics::render_object_ref_t rc ) noexcept ;

        } ;
        natus_res_typedef( line_render_3d ) ;
    }
}