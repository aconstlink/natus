
#pragma once

#include "../object.hpp"

#include "../shader/shader_configuration.h"
#include "../buffer/vertex_attribute.h"
#include "../variable/variable_set.hpp"

#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API render_configuration : public object
        {
            natus_this_typedefs( render_configuration ) ;

        private:

            natus::ntd::string_t _name ;
            natus::ntd::string_t _geo ;
            natus::ntd::string_t _shader ;

            natus::ntd::vector< natus::gpu::variable_set_res_t > _vars ;

        public:
            
            render_configuration( void_t ) {}
            render_configuration( natus::ntd::string_cref_t name ) 
                : _name( name ) {}

            render_configuration( this_cref_t rhv ) : object( rhv )
            {
                _name = rhv._name ;
                _geo = rhv._geo;
                _shader = rhv._shader ;
                _vars = rhv._vars ;
            }

            render_configuration( this_rref_t rhv ) : object( ::std::move( rhv ) )
            {
                _name = ::std::move( rhv._name ) ;
                _geo = ::std::move( rhv._geo ) ;
                _shader = ::std::move( rhv._shader ) ;
                _vars = ::std::move( rhv._vars ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                object::operator=( rhv ) ;

                _name = rhv._name ;
                _geo = rhv._geo;
                _shader = rhv._shader ;
                _vars = rhv._vars ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                object::operator=( ::std::move( rhv ) ) ;

                _name = ::std::move( rhv._name ) ;
                _geo = ::std::move( rhv._geo ) ;
                _shader = ::std::move( rhv._shader ) ;
                _vars = ::std::move( rhv._vars ) ;

                return *this ;
            }

        public:

            // render states
            // textures
            
            this_ref_t link_geometry( natus::ntd::string_cref_t name ) noexcept 
            {
                _geo = name ;
                return *this ;
            }

            natus::ntd::string_cref_t get_geometry( void_t ) const noexcept
            {
                return _geo ;
            }

            this_ref_t link_shader( natus::ntd::string_cref_t name ) noexcept
            {
                _shader = name ;
                return *this ;
            }

            natus::ntd::string_cref_t get_shader( void_t ) const noexcept
            {
                return _shader ;
            }

            this_ref_t add_variable_set( natus::gpu::variable_set_res_t vs )
            {
                _vars.emplace_back( ::std::move( vs ) ) ;
                return *this ;
            }

            typedef ::std::function< void_t ( size_t const i, natus::gpu::variable_set_res_t const & ) > for_each_var_funk_t ;

            void_t for_each( for_each_var_funk_t funk )
            {
                size_t i = 0 ;
                for( auto const & v : _vars )
                {
                    funk( i++, v ) ;
                }
            }

        public:

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }
        };
        natus_res_typedef( render_configuration ) ;
    }
}
