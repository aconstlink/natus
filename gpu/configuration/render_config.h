
#pragma once

#include "../api.h"
#include "../result.h"
#include "../protos.h"

#include "../backend/id.hpp"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API render_config 
        {
            natus_this_typedefs( render_config ) ;

        private:

            // shaders
            vertex_shader_rptr_t _vs ;
            geometry_shader_rptr_t _gs ;
            pixel_shader_rptr_t _ps ;

            // render state set
            // variable set


        public:

            render_config( void_t ) {
            }

            render_config( this_cref_t ) = delete ;

            render_config( this_rref_t rhv ) : backend_id( ::std::move( rhv) )
            {
                _vs = ::std::move( rhv._vs ) ;
                _gs = ::std::move( rhv._gs ) ;
                _ps = ::std::move( rhv._ps ) ;
            }

            virtual ~render_config( void_t ) {}


        };
        natus_typedef( render_config ) ;
    }
}