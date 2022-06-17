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

            virtual void_t update_view_matrix( natus::math::mat4f_cref_t frame ) noexcept = 0 ;
            virtual natus::math::mat4f_cref_t get_view_matrix( void_t ) const  noexcept = 0 ;
            virtual natus::math::mat4f_cref_t get_proj_matrix( void_t ) const  noexcept = 0 ;
        };
        natus_res_typedef( ilens ) ;
        typedef natus::memory::res_t< ilens_t > lens_res_t ;
    }
}