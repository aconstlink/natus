
#pragma once

#include "../api.h"
#include "../protos.h"
#include "../typedefs.h"

#include "../shader/shader_configuration.h"
#include "../buffer/vertex_attribute.h"

#include <natus/std/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API render_configuration
        {
            natus_this_typedefs( render_configuration ) ;

        private:

            natus::std::string_t _name ;
            natus::std::string_t _geo ;
            natus::std::string_t _shader ;

        public:
            
            render_configuration( void_t ) {}
            render_configuration( natus::std::string_cref_t name ) 
                : _name( name ) {}

            render_configuration( this_cref_t rhv )
            {
                *this = rhv  ;
            }

            render_configuration( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

        public:

            // render states
            // textures
            
            this_ref_t link_geometry( natus::std::string_cref_t name ) noexcept 
            {
                _geo = name ;
                return *this ;
            }

            natus::std::string_cref_t get_geometry( void_t ) const noexcept
            {
                return _geo ;
            }

            this_ref_t link_shader( natus::std::string_cref_t name ) noexcept
            {
                _shader = name ;
                return *this ;
            }

            natus::std::string_cref_t get_shader( void_t ) const noexcept
            {
                return _shader ;
            }

        public:

            this_ref_t operator = ( this_cref_t rhv )
            {
                _name = rhv._name ;
                _geo = rhv._geo;
                _shader = rhv._shader ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _name = ::std::move( rhv._name ) ;
                _geo = ::std::move( rhv._geo ) ;
                _shader = ::std::move( rhv._shader ) ;

                return *this ;
            }

            natus::std::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }
        };
        natus_soil_typedef( render_configuration ) ;
    }
}
