
#pragma once

#include "generic_camera.h"
#include "pinhole_lens.h"

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API pinhole_camera
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

            natus::math::vec3f_t get_position( void_t ) const noexcept
            {
                return _camera->get_transformation().get_translation() ;
            }

            this_ref_t look_at( natus::math::vec3f_cref_t pos,
                natus::math::vec3f_cref_t up, natus::math::vec3f_cref_t at ) noexcept
            {
                _camera->for_each_lens( [&] ( icamera_ref_t, lens_res_t& l ) 
                { 
                    l->look_at( pos, up, at ) ;
                } ) ;
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
        };
        natus_res_typedef( pinhole_camera ) ;
    }
}