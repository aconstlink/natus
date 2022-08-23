#pragma once

#include "icamera.h"
#include "ilens.h"

#include <natus/math/primitive/3d/ray.hpp>
#include <natus/math/primitive/3d/frustum.hpp>
#include <natus/math/utility/3d/transformation.hpp>
#include <natus/math/camera/3d/perspective_fov.hpp>
#include <natus/ntd/vector.hpp>

#include <array>

namespace natus
{
    namespace gfx
    {
        // A generic camera class storing lenses for rendering 
        // through those lenses. The camera has a frame for spacial 
        // positioning and the lenes have a transformation too.
        // @todo each lens needs an offset so a stereoscopic camera
        // can be created by adding two lenses and defining the offset.
        class NATUS_GFX_API generic_camera
        {
            natus_this_typedefs( generic_camera ) ;
            natus_typedefs( natus::ntd::vector<lens_res_t>, lenses ) ;

            natus_typedefs( natus::math::m3d::ray3f_t, ray3 ) ;
            natus_typedefs( natus::math::vec2f_t, vec2 ) ;
            natus_typedefs( natus::math::vec3f_t, vec3 ) ;
            natus_typedefs( natus::math::m3d::frustum<float_t>, frustum ) ;
            natus_typedefs( frustum_t::plane_t, plane ) ;

        private:

            enum class projection_type
            {
                undefined,
                orthographic,
                perspective
            };

            projection_type _projection_mode ;

        private:

            natus::math::mat4f_t _cam_matrix ;
            natus::math::mat4f_t _view_matrix ;
            natus::math::mat4f_t _proj_matrix ;

            float_t _fov ;
            float_t _aspect ;

            // in pixels
            natus::math::vec2f_t _sensor_dims ;
            natus::math::vec2f_t _near_far ;
            frustum_t _frustum ;

        private:

            natus::math::m3d::trafof_t _trafo ;
            lenses_t _lenses ;

        public: // 

            generic_camera( void_t ) noexcept ;

            generic_camera( natus::gfx::lens_res_t lens ) noexcept ;

            generic_camera( this_rref_t rhv ) noexcept ;

            generic_camera( this_cref_t rhv ) noexcept ;
            
            virtual ~generic_camera( void_t ) noexcept ;

        public:

            natus::gfx::result add_lens( natus::gfx::lens_res_t lens ) noexcept ;
            void_t replace_lens( size_t const, lens_res_t ) noexcept ;

        public:

            this_ref_t make_orthographic( float_t const w, float_t const h,
                float_t const n, float_t const f ) noexcept ;

            this_ref_t make_perspective_fov( float_t const w, float_t const h,
                float_t const fov, float_t const aspect,
                float_t const n, float_t const f ) noexcept  ;

            static this_t create_orthographic( float_t const w, float_t const h,
                float_t const n, float_t const f ) noexcept ;

            static this_t create_perspective_fov( float_t const w, float_t const h, 
                float_t const fov, float_t const aspect,
                float_t const n, float_t const f ) noexcept  ;

            bool_t is_perspective( void_t ) const noexcept ;

            bool_t is_orthographic( void_t ) const noexcept ;

            void_t update_view_matrix( natus::math::mat4f_cref_t frame ) noexcept ;

            natus::math::mat4f_cref_t get_view_matrix( void_t ) const noexcept ;

            natus::math::mat4f_cref_t get_proj_matrix( void_t ) const noexcept ;

        public: // get

            vec3_t get_position( void_t ) const noexcept ;

            this_cref_t get_position( vec3_ref_t pos ) const noexcept ;

            vec3_t get_direction( void_t ) const noexcept ;

            this_cref_t get_direction( vec3_ref_t dir ) const noexcept ;

            vec3_t get_up( void_t ) const noexcept ;

            this_cref_t get_up( vec3_ref_t up ) const noexcept ;

            vec3_t get_right( void_t ) const noexcept ;

            this_cref_t get_right( vec3_ref_t right ) const noexcept ;

            frustum_cref_t get_frustum( void_t ) const noexcept ;

        public: // ray

            /// creates a ray in view center direction.
            /// @param norm_pos [in] [-1,1] screen position
            ray3_t create_ray_norm( vec2_cref_t norm_pos ) const noexcept ;

            /// creates a ray in view center direction.
            ray3_t create_center_ray( void_t ) const ;

            /// creates a ray in view center direction.
            /// @param screen_pos [in] [(0,0),(width,height)] screen position.
            ray3_t create_ray( vec2_cref_t screen_pos ) const ;

        public: // interface

            
            size_t get_num_lenses( void_t ) const noexcept ;

            lens_res_t get_lens( size_t const i ) noexcept ;

            void_t transform_by( natus::math::m3d::trafof_cref_t trafo ) noexcept  ;
            void_t set_transformaion( natus::math::m3d::trafof_cref_t trafo ) noexcept ;

            natus::math::m3d::trafof_cref_t get_transformation( void_t ) const noexcept ;

            typedef std::function< void_t ( this_ref_t, lens_res_t& ) > lens_funk_t ;
            void_t for_each_lens( lens_funk_t funk ) noexcept  ;

        private:

            
            void_t reconstruct_frustum_planes( void_t ) noexcept ;

        };
        natus_res_typedef( generic_camera ) ;
    }
}