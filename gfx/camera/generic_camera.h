#pragma once

#include "icamera.h"
#include "ilens.h"

#include <natus/math/utility/3d/transformation.hpp>
#include <natus/std/vector.hpp>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API generic_camera : public icamera
        {
            natus_this_typedefs( generic_camera ) ;
            natus_typedefs( natus::std::vector<lens_res_t>, lenses ) ;

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

            natus::gfx::result add_lens( natus::gfx::lens_res_t lens ) ;

        public: // interface

            virtual size_t get_num_lenses( void_t ) const ;

            virtual lens_res_t get_lens( size_t const i ) ;

            virtual void_t transform_by( natus::math::m3d::trafof_cref_t trafo )  ;
            virtual void_t set_transformaion( natus::math::m3d::trafof_cref_t trafo ) ;

            virtual natus::math::m3d::trafof_cref_t get_transformation( void_t ) const ;

            typedef ::std::function< void_t ( icamera_ref_t, lens_res_t& ) > lens_funk_t ;
            virtual void_t for_each_lens( lens_funk_t funk )  ;
        };
        natus_soil_typedef( generic_camera ) ;
    }
}