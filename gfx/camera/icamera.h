#pragma once

#include "../typedefs.h"
#include "../result.h"
#include "../api.h"
#include "../protos.h"
#include "ilens.h"

#include <natus/math/vector/vector3.hpp>
#include <natus/math/utility/3d/transformation.hpp>

#include <functional>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API icamera
        {
        public:

            virtual size_t get_num_lenses( void_t ) const = 0 ;
            virtual lens_res_t get_lens( size_t const ) = 0 ;

            typedef ::std::function< void_t ( icamera_ref_t, lens_res_t & ) > lens_funk_t ;
            virtual void_t for_each_lens( lens_funk_t ) = 0 ;

        public:

            virtual void_t transform_by( natus::math::m3d::trafof_cref_t ) = 0 ;
            virtual void_t set_transformaion( natus::math::m3d::trafof_cref_t ) = 0 ;
            virtual natus::math::m3d::trafof_cref_t get_transformation( void_t ) const = 0 ;
        };
    }
}