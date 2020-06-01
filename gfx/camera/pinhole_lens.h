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

            pinhole_lens( void_t ) 
            {
                _lens_matrix.identity() ;
                _view_matrix.identity() ;
                _proj_matrix.identity() ;
            }
            pinhole_lens( this_rref_t rhv ) 
            {
                _lens_frame = rhv._lens_frame ;
                _lens_matrix = rhv._lens_matrix ;
                _view_matrix = rhv._view_matrix ;
                _proj_matrix = rhv._proj_matrix ;

                _projection_mode = rhv._projection_mode ;
            }
            virtual ~pinhole_lens( void_t ) {}

        public:

            this_ref_t make_orthographic( float_t const w, float_t const h,
                float_t const near, float_t const far ) noexcept 
            {
                _proj_matrix = natus::math::m3d::orthographic<float_t>::create(
                    w, h, near, far ) ;

                _projection_mode = projection_type::orthographic ;

                return *this ;
            }

            this_ref_t make_perspective_fov( float_t const fov, float_t const aspect,
                float_t const near, float_t const far ) noexcept 
            {
                natus::math::m3d::perspective<float_t>::create_by_fovv_aspect(
                    fov, aspect, near, far, _proj_matrix ) ;

                _projection_mode = projection_type::perspective ;

                return *this ;
            }

            static this_t create_orthographic( float_t const w, float_t const h,
                float_t const near, float_t const far ) noexcept 
            {
                return ::std::move( this_t().make_orthographic( w, h, near, far ) ) ;
            }

            static this_t create_perspective_fov( float_t const fov, float_t const aspect,
                float_t const near, float_t const far ) noexcept 
            {
                this_t ret ;

                natus::math::m3d::perspective<float_t>::create_by_fovv_aspect(
                    fov, aspect, near, far, ret._proj_matrix ) ;

                ret._projection_mode = projection_type::perspective ;

                return ::std::move( ret ) ;
            }

            bool_t is_perspective( void_t ) const
            {
                return _projection_mode == projection_type::perspective ;
            }

            bool_t is_orthographic( void_t ) const
            {
                return _projection_mode == projection_type::orthographic ;
            }

            void_t set_camera_matrix( natus::math::mat4f_cref_t ) ;

        public: // interface

            virtual natus::math::vec3f_t get_position( void_t ) const 
            {
                return _lens_matrix.get_column3( 3 ) ;
            }

            virtual natus::math::mat3f_cref_t get_lens_frame( void_t ) const 
            {
                return _lens_frame ;
            }

            virtual natus::math::mat4f_cref_t get_lens_matrix( void_t ) const 
            {
                return _lens_matrix ;
            }

            virtual natus::math::mat4f_cref_t get_view_matrix( void_t ) const 
            {
                return _view_matrix ;
            }

            virtual natus::math::mat4f_cref_t get_proj_matrix( void_t ) const 
            {
                return _proj_matrix ;
            }

            virtual void_t transform_by( natus::math::m3d::trafof_cref_t trafo_in ) 
            {
                natus::math::m3d::trafof_t const trafo =
                    natus::math::m3d::trafof_t( _lens_matrix ) * trafo_in ;

                natus::math::mat4f_t const mat_comp = trafo.get_transformation() * _lens_matrix ;

                natus::math::m3d::create_view_matrix( mat_comp, _view_matrix ) ;
            }

            virtual void_t set_transformation( natus::math::m3d::trafof_cref_t trafo_in ) 
            {
                _lens_matrix = trafo_in.get_transformation() ;
                natus::math::m3d::create_view_matrix( _lens_matrix, _view_matrix ) ;
            }

            virtual void_t look_at( natus::math::vec3f_cref_t pos,
                natus::math::vec3f_cref_t up, natus::math::vec3f_cref_t at ) 
            {
                natus::math::m3d::create_lookat_rh( pos, up, at, _lens_matrix ) ;
                natus::math::m3d::create_view_matrix( _lens_matrix, _view_matrix ) ;
            }
        };
        natus_soil_typedef( pinhole_lens ) ;
    }
}