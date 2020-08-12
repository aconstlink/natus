#pragma once

#include "../typedefs.h"
#include "../protos.h"
#include "../api.h"

#include <natus/math/vector/vector3.hpp>
#include <natus/math/matrix/matrix3.hpp>
#include <natus/math/matrix/matrix4.hpp>
#include <natus/math/utility/3d/transformation.hpp>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API ilens
        {
        public:

            /// lens position relative to camera
            virtual natus::math::vec3f_t get_position( void_t ) const = 0 ;

            virtual natus::math::mat3f_cref_t get_lens_frame( void_t ) const = 0 ;
            virtual natus::math::mat4f_cref_t get_lens_matrix( void_t ) const = 0 ;
            virtual natus::math::mat4f_cref_t get_view_matrix( void_t ) const = 0 ;
            virtual natus::math::mat4f_cref_t get_proj_matrix( void_t ) const = 0 ;

            virtual void_t transform_by( natus::math::m3d::trafof_cref_t )  = 0 ;
            virtual void_t set_transformation( natus::math::m3d::trafof_cref_t ) = 0 ;

            virtual void_t look_at( natus::math::vec3f_cref_t pos,
                natus::math::vec3f_cref_t up, natus::math::vec3f_cref_t at ) = 0 ;
        };
        natus_res_typedef( ilens ) ;
        typedef natus::memory::res< ilens_t > lens_res_t ;
    }
}