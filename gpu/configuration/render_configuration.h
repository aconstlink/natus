
#pragma once

#include "../api.h"
#include "../protos.h"
#include "../typedefs.h"

#include "../shader/vertex_shader.hpp"
#include "../shader/geometry_shader.hpp"
#include "../shader/pixel_shader.hpp"

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

            natus::gpu::backend_type _type ;
            natus::gpu::vertex_shader_t _vs ;
            natus::gpu::pixel_shader_t _ps ;

        public:

            render_configuration( void_t ) {}
            render_configuration( natus::gpu::backend_type const bt ) : _type(bt) {}
            render_configuration( this_cref_t rhv )
            {
                *this = rhv  ;
            }

            render_configuration( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

        public:
            
            natus::gpu::backend_type get_type( void_t ) const noexcept { return _type ; }
            void_t set_type( natus::gpu::backend_type const bt ) noexcept { _type = bt ; }


            // render states
            
            // geometry
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
            natus::gpu::pixel_shader_cref_t get_pixel_shader( void_t ) const noexcept { return _ps ; }

        public:

            this_ref_t operator = ( this_cref_t rhv )
            {
                _vs = rhv._vs ;
                _ps = rhv._ps ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _vs = ::std::move( rhv._vs ) ;
                _ps = ::std::move( rhv._ps ) ;

                return *this ;
            }
        };
        natus_typedef( render_configuration ) ;

        /// per backend type render configuration
        class render_configurations
        {
            natus_this_typedefs( render_configurations ) ;

        private:

            struct data
            {
                natus::gpu::render_configuration_t config ;
            };
            natus_typedef( data ) ;

        private:

            typedef natus::std::vector< data > configs_t ;
            configs_t _configs ;

        public:

            render_configurations( void_t ) {}
            render_configurations( natus::gpu::render_configuration_in_t config ) 
            {
                _configs.emplace_back( this_t::data { config } ) ;
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
            void_t add_config( natus::gpu::render_configuration_in_t config )
            {
                auto iter = ::std::find_if( _configs.begin(), _configs.end(), 
                    [&] ( this_t::data_in_t d ) { 
                    return d.config.get_type() == config.get_type() ; } ) ;

                if( iter != _configs.end() )
                {
                    iter->config = config ;
                    return ;
                }

                _configs.emplace_back( this_t::data_t { config } ) ;
            }

            //***
            void_t add_config( natus::gpu::backend_type const bt,
                natus::gpu::render_configuration_rref_t config )
            {
                auto iter = ::std::find_if( _configs.begin(), _configs.end(),
                    [&] ( this_t::data_in_t d ) {
                    return d.config.get_type() == config.get_type() ; } ) ;

                if( iter != _configs.end() )
                {
                    iter->config = ::std::move( config )  ;
                    return ;
                }

                _configs.emplace_back( this_t::data_t { ::std::move( config ) } ) ;
            }

            //***
            bool_t get_configuration( natus::gpu::render_configuration_out_t config ) const noexcept
            {
                auto iter = ::std::find_if( _configs.begin(), _configs.end(), 
                    [&] ( this_t::data_in_t d ) {
                    return d.config.get_type() == config.get_type() ; } ) ;

                if( iter != _configs.end() )
                {
                    config = iter->config ;
                }

                return iter != _configs.end() ;
            }
        };
        natus_typedef( render_configurations ) ;
    }
}
