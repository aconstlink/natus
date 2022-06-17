#pragma once

#include "ilens.h"

#include <natus/math/utility/3d/look_at.hpp>
#include <natus/math/utility/3d/perspective_fov.hpp>
#include <natus/math/utility/3d/orthographic_projection.hpp>
#include <natus/math/utility/3d/transformation.hpp>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API pinhole_lens : public ilens
        {
            natus_this_typedefs( pinhole_lens ) ;

        private:

            enum class projection_type
            {
                undefined,
                orthographic,
                perspective
            };

        private:

            natus::math::mat3f_t _lens_frame ;
            natus::math::mat4f_t _lens_matrix ;
            natus::math::mat4f_t _view_matrix ;
            natus::math::mat4f_t _proj_matrix ;

            projection_type _projection_mode ;

        public:

            pinhole_lens( void_t ) noexcept
            {
                _lens_matrix.identity() ;
                _view_matrix.identity() ;
                _proj_matrix.identity() ;
            }
            pinhole_lens( this_rref_t rhv ) noexcept
            {
                _lens_frame = rhv._lens_frame ;
                _lens_matrix = rhv._lens_matrix ;
                _view_matrix = rhv._view_matrix ;
                _proj_matrix = rhv._proj_matrix ;

                _projection_mode = rhv._projection_mode ;
            }
            virtual ~pinhole_lens( void_t ) noexcept {}

        public:

            this_ref_t make_orthographic( float_t const w, float_t const h,
                float_t const n, float_t const f ) noexcept 
            {
                _proj_matrix = natus::math::m3d::orthographic<float_t>::create(
                    w, h, n, f ) ;

                _projection_mode = projection_type::orthographic ;

                return *this ;
            }

            this_ref_t make_perspective_fov( float_t const fov, float_t const aspect,
                float_t const n, float_t const f ) noexcept 
            {
                natus::math::m3d::perspective<float_t>::create_by_fovv_aspect(
                    fov, aspect, n, f, _proj_matrix ) ;

                _projection_mode = projection_type::perspective ;

                return *this ;
            }

            static this_t create_orthographic( float_t const w, float_t const h,
                float_t const n, float_t const f ) noexcept 
            {
                return ::std::move( this_t().make_orthographic( w, h, n, f ) ) ;
            }

            static this_t create_perspective_fov( float_t const fov, float_t const aspect,
                float_t const n, float_t const f ) noexcept 
            {
                this_t ret ;

                natus::math::m3d::perspective<float_t>::create_by_fovv_aspect(
                    fov, aspect, n, f, ret._proj_matrix ) ;

                ret._projection_mode = projection_type::perspective ;

                return ::std::move( ret ) ;
            }

            bool_t is_perspective( void_t ) const noexcept
            {
                return _projection_mode == projection_type::perspective ;
            }

            bool_t is_orthographic( void_t ) const noexcept
            {
                return _projection_mode == projection_type::orthographic ;
            }

        public: // interface

            virtual void_t update_view_matrix( natus::math::mat4f_cref_t frame ) noexcept 
            {
                natus::math::m3d::create_view_matrix( frame, _view_matrix ) ;
            }

            virtual natus::math::mat4f_cref_t get_view_matrix( void_t ) const noexcept 
            {
                return _view_matrix ;
            }

            virtual natus::math::mat4f_cref_t get_proj_matrix( void_t ) const noexcept 
            {
                return _proj_matrix ;
            }
        };
        natus_res_typedef( pinhole_lens ) ;
    }
}