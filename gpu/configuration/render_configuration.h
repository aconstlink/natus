
#pragma once

#include "../api.h"
#include "../protos.h"
#include "../typedefs.h"

#include "../shader/vertex_shader.hpp"
#include "../shader/geometry_shader.hpp"
#include "../shader/pixel_shader.hpp"

#include "../variable/variable_set.hpp"

#include "../backend/types.h"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API render_configuration
        {
            natus_this_typedefs( render_configuration ) ;

        private:

            natus::gpu::vertex_shader_t _vs ;
            natus::gpu::geometry_shader_t _gs ;
            natus::gpu::pixel_shader_t _ps ;

            natus::std::string_t _geo ;
            natus::std::string_t _name ;

            natus::gpu::variable_set_t _var_set ;

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
            
            // geometry
            this_ref_t set_geometry( natus::std::string_cref_t name ) noexcept 
            {
                _geo = name ;
                return *this ;
            }

            natus::std::string_cref_t get_geometry( void_t ) const noexcept
            {
                return _geo ;
            }

            // textures

            // shaders
            natus::gpu::vertex_shader_cref_t set_shader( natus::gpu::vertex_shader_cref_t s ) noexcept
            {
                _vs = s ;
                return _vs ;
            }

            natus::gpu::pixel_shader_cref_t set_shader( natus::gpu::pixel_shader_cref_t s ) noexcept
            {
                _ps = s ;
                return _ps ;
            }

            natus::gpu::vertex_shader_cref_t get_vertex_shader( void_t ) const noexcept { return _vs ; }
            natus::gpu::geometry_shader_cref_t get_geometry_shader( void_t ) const noexcept { return _gs ; }
            natus::gpu::pixel_shader_cref_t get_pixel_shader( void_t ) const noexcept { return _ps ; }

            bool_t has_vertex_shader( void_t ) const noexcept
            {
                return natus::core::is_not( _vs.code().empty() ) ;
            }

            bool_t has_geometry_shader( void_t ) const noexcept 
            {
                return natus::core::is_not( _gs.code().empty() ) ; 
            }

            bool_t has_pixel_shader( void_t ) const noexcept
            {
                return natus::core::is_not( _ps.code().empty() ) ;
            }

        public:

            this_ref_t operator = ( this_cref_t rhv )
            {
                _vs = rhv._vs ;
                _ps = rhv._ps ;
                _name = rhv._name ;
                _geo = rhv._geo;
                _var_set = rhv._var_set ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _vs = ::std::move( rhv._vs ) ;
                _ps = ::std::move( rhv._ps ) ;
                _name = ::std::move( rhv._name ) ;
                _geo = ::std::move( rhv._geo ) ;
                _var_set = ::std::move( rhv._var_set ) ;
                return *this ;
            }

            natus::std::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }

            template< class T >
            ::std::shared_ptr< natus::gpu::variable< T > > create_variable( natus::std::string_cref_t name ) noexcept 
            {
                return _var_set.create_variable< T >( name ) ;
            }
        };
        natus_soil_typedef( render_configuration ) ;

        /// per backend type render configuration
        class render_configurations
        {
            natus_this_typedefs( render_configurations ) ;

        private:

            struct data
            {
                natus::gpu::backend_type bt ;
                natus::gpu::render_configuration_res_t config ;
            };
            natus_typedef( data ) ;

            typedef natus::std::vector< data_t > configs_t ;
            configs_t _configs ;

        public:

            render_configurations( void_t ) {}
            render_configurations( natus::gpu::backend_type const bt, 
                natus::gpu::render_configuration_res_t config ) 
            {
                _configs.emplace_back( this_t::data_t( { bt, config } ) ) ;
            }

            render_configurations( this_cref_t rhv )
            {
                _configs = rhv._configs ;
            }

            render_configurations( this_rref_t rhv ) 
            {
                _configs = ::std::move( rhv._configs ) ;
            }

        public:

            //***
            void_t add_config( natus::gpu::backend_type const bt, natus::gpu::render_configuration_res_t config )
            {
                auto iter = ::std::find_if( _configs.begin(), _configs.end(), 
                    [&] ( data_cref_t d ) { return d.bt == bt ; } ) ;

                if( iter != _configs.end() )
                {
                    iter->config = config ;
                    return ;
                }

                _configs.emplace_back( this_t::data_t( {bt, config} ) ) ;
            }

            //***
            void_t add_config( natus::gpu::backend_type const bt, natus::gpu::render_configuration_res_t && config )
            {
                auto iter = ::std::find_if( _configs.begin(), _configs.end(),
                    [&] ( data_cref_t d ) { return d.bt == bt ; } ) ;

                if( iter != _configs.end() )
                {
                    iter->config = ::std::move( config )  ;
                    return ;
                }

                _configs.emplace_back( this_t::data_t( { bt, ::std::move( config ) } )  ) ;
            }

            //***
            bool_t find_configuration( natus::gpu::backend_type const bt, 
                natus::gpu::render_configuration_res_t & config ) const noexcept
            {
                auto iter = ::std::find_if( _configs.begin(), _configs.end(),
                    [&] ( data_cref_t d ) { return d.bt == bt ; } ) ;

                if( iter != _configs.end() )
                {
                    config = iter->config ;
                }

                return iter != _configs.end() ;
            }
        };
        natus_soil_typedef( render_configurations ) ;
    }
}
