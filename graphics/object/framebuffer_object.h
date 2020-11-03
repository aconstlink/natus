
#pragma once

#include "../object.hpp"
#include "../types.h"

#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API framebuffer_object : public object
        {
            natus_this_typedefs( framebuffer_object ) ;

        private:

            natus::ntd::string_t _name ;

            struct render_target
            {
                natus::graphics::render_target_type rtt = natus::graphics::render_target_type::unknown ;
            };

            render_target _targets[ 8 ] ;

        public:

            framebuffer_object( void_t ) {}

            framebuffer_object( natus::ntd::string_cref_t name )
                : _name( name ) {}

            ~framebuffer_object( void_t ) {}

        public:

            this_ref_t set_render_target( size_t const i, natus::graphics::render_target_type const t ) noexcept
            {
                if( i >= 8 )
                {
                    natus::log::global_t::error("Render target index invalid : " + std::to_string( i ) ) ;
                    return *this ;
                }

                _targets[ i ] = { t } ;

                return *this ;
            }

        } ;
        natus_res_typedef( framebuffer_object ) ;
    }
}

