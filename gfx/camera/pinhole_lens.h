#pragma once

#include "ilens.h"



namespace natus
{
    namespace gfx
    {
        // very simplistic lens. Has no physical properties.
        class NATUS_GFX_API pinhole_lens : public ilens
        {
            natus_this_typedefs( pinhole_lens ) ;

        public:

            pinhole_lens( void_t ) noexcept{}
            pinhole_lens( this_rref_t rhv ) noexcept{}
            virtual ~pinhole_lens( void_t ) noexcept {}

        public: // interface

        };
        natus_res_typedef( pinhole_lens ) ;
    }
}