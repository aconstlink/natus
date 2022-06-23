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
        class NATUS_GFX_API tri_render_3d
        {
            natus_this_typedefs( tri_render_3d ) ;

        private: // data

            struct tri
            {
                struct points
                {
                    natus::math::vec3f_t p0 ;
                    natus::math::vec3f_t p1 ;
                    natus::math::vec3f_t p2 ;
                };

                union 
                {
                    points pts ;
                    natus::math::vec3f_t array[3] ;
                };
                natus::math::vec4f_t color ;

                tri( void_t ) noexcept {}
                tri( tri const & rhv ) noexcept
                {
                    std::memcpy( reinterpret_cast<void_ptr_t>(this),
                        reinterpret_cast<void_cptr_t>(&rhv), sizeof(tri) ) ;
                }
                tri( tri && rhv ) noexcept
                {
                    std::memcpy( reinterpret_cast<void_ptr_t>(this),
                        reinterpret_cast<void_cptr_t>(&rhv), sizeof(tri) ) ;
                }

                tri & operator = ( tri const & rhv ) noexcept
                {
                    std::memcpy( reinterpret_cast<void_ptr_t>(this),
                        reinterpret_cast<void_cptr_t>(&rhv), sizeof(tri) ) ;
                    return *this ;
                }
                tri & operator = ( tri && rhv ) noexcept
                {
                    std::memcpy( reinterpret_cast<void_ptr_t>(this),
                        reinterpret_cast<void_cptr_t>(&rhv), sizeof(tri) ) ;
                    return *this ;
                }
            };
            natus_typedef( tri ) ;

            natus::concurrent::mutex_t _tris_mtx ;
            natus::ntd::vector< tri_t > _tris ;
            size_t _num_tris = 0 ;

        private: // graphics

            struct vertex
            {
                natus::math::vec3f_t pos ;
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

            natus::math::mat4f_t _proj ;
            natus::math::mat4f_t _view ;

        public:

            tri_render_3d( void_t ) noexcept ;
            tri_render_3d( this_cref_t ) = delete ;
            tri_render_3d( this_rref_t ) noexcept ;
            ~tri_render_3d( void_t ) noexcept ;

            void_t init( natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
            void_t release( void_t ) noexcept ;

        public:

            void_t draw( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, 
                natus::math::vec3f_cref_t p2, natus::math::vec4f_cref_t color ) noexcept ;

            void_t draw_rect( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, 
                natus::math::vec3f_cref_t p2, natus::math::vec3f_cref_t p3, natus::math::vec4f_cref_t color ) noexcept ;

            void_t draw_circle( natus::math::mat3f_cref_t, natus::math::vec3f_cref_t p0, float_t const r, natus::math::vec4f_cref_t color, size_t const ) noexcept ;

        public:

            // copy all data to the gpu buffer and transmit the data
            void_t prepare_for_rendering( void_t ) noexcept ;
            void_t render( void_t ) noexcept ;

            void_t set_view_proj( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t ) noexcept ;

        private:

            natus_typedefs( natus::ntd::vector< natus::math::vec3f_t >, circle ) ;
            natus::ntd::vector< circle_t > _circle_cache ;
            circle_cref_t lookup_circle_cache( size_t const ) noexcept ;

            void_t add_variable_set( natus::graphics::render_object_ref_t rc ) noexcept ;

        };
        natus_res_typedef( tri_render_3d ) ;
    }
}