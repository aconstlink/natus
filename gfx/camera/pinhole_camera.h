
#pragma once

#include "generic_camera.h"
#include "pinhole_lens.h"

#include <natus/math/utility/3d/look_at.hpp>

namespace natus
{
    namespace gfx
    {
        // this class simplyfies the use of the pinholse lens and 
        // the generic camera. It has one pinhole lens that can 
        // be directly accessed through this class.
        class pinhole_camera
        {
            natus_this_typedefs( pinhole_camera ) ;

            natus::gfx::pinhole_lens_res_t _lens ;
            natus::gfx::generic_camera_res_t _camera ;

        public:

            pinhole_camera( void_t ) noexcept
            {
                _lens = natus::gfx::pinhole_lens_res_t( natus::gfx::pinhole_lens_t() ) ;
                _camera = natus::gfx::generic_camera_res_t( natus::gfx::generic_camera_t() ) ;

                _camera->add_lens( _lens ) ;
            }

            pinhole_camera( this_rref_t rhv ) noexcept
            {
                *this = ::std::move( rhv ) ;
            }

            pinhole_camera( this_cref_t rhv ) = delete ;

            ~pinhole_camera( void_t ) noexcept
            {}

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _lens = ::std::move( rhv._lens ) ;
                _camera = ::std::move( rhv._camera ) ;
                return *this ;
            }

        public:

            this_ref_t orthographic( float_t const w, float_t const h,
                float_t const n, float_t const f  ) noexcept
            {
                _lens->make_orthographic( w, h, n, f ) ;
                return *this ;
            }

            this_ref_t perspective_fov( float_t const fov, float_t const aspect,
                float_t const n, float_t const f  ) noexcept
            {
                _lens->make_perspective_fov( fov, aspect, n, f ) ;
                return *this ;
            }

            this_ref_t translate_by( natus::math::vec3f_cref_t d ) noexcept
            {
                natus::math::m3d::trafof_t t ;
                t.translate_fr( d ) ;
                _camera->transform_by( t ) ;
                return *this ;
            }

            this_ref_t translate_to( natus::math::vec3f_cref_t d ) noexcept
            {
                auto t = _camera->get_transformation() ;
                t.set_translation( d ) ;
                _camera->set_transformaion( t ) ;
                return *this ;
            }

            natus::math::vec3f_t get_position( void_t ) const noexcept
            {
                return _camera->get_transformation().get_translation() ;
            }

            this_ref_t look_at( natus::math::vec3f_cref_t pos,
                natus::math::vec3f_cref_t up, natus::math::vec3f_cref_t at ) noexcept
            {
                natus::math::mat4f_t m ;
                natus::math::m3d::create_lookat_rh( pos, up, at, m ) ;

                _camera->set_transformaion( m ) ;

                return *this ;
            }

        public:

            natus::math::mat4f_t mat_proj( void_t ) const noexcept
            {
                return _lens->get_proj_matrix() ;
            }

            natus::math::mat4f_t mat_view( void_t ) const noexcept
            {
                return _lens->get_view_matrix() ;
            }

            natus::math::m3d::trafof_cref_t get_transformation( void_t ) const noexcept
            {
                return _camera->get_transformation() ;
            }

            void_t set_transformation( natus::math::m3d::trafof_cref_t t ) noexcept
            {
                _camera->set_transformaion( t ) ;
            }
        };
        natus_res_typedef( pinhole_camera ) ;
    }
}