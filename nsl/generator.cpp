
#include "generator.h"

using namespace natus::nsl ;

natus::nsl::generated_code_t generator::generate( void_t ) noexcept
{
    natus::nsl::generated_code_t ret ;

    natus::nsl::variable_mappings_t mappings ;

    natus::nsl::generated_code_t::shader_t shd ;
    {
        for( auto const& s : _genable.config.shaders )
        {
            natus::nsl::shader_type const s_type = s.type ;

            if( s_type == natus::nsl::shader_type::unknown )
            {
                natus::log::global_t::warning( "[generator] : unknown shader type" ) ;
                continue;
            }

            for( auto const& v : s.variables )
            {
                natus::nsl::variable_mapping_t vm ;
                vm.st = s_type ;

                // everything is var first
                natus::ntd::string_t flow = "var_" ;

                if( v.fq == natus::nsl::flow_qualifier::global )
                {
                    flow = "" ;
                } else if( s.type == natus::nsl::shader_type::vertex_shader &&
                    v.fq == natus::nsl::flow_qualifier::in )
                {
                    flow = "in_" ;
                } else if( s.type == natus::nsl::shader_type::pixel_shader &&
                    v.fq == natus::nsl::flow_qualifier::out )
                {
                    flow = "out_" ;
                }
                vm.new_name = flow + v.name ;
                vm.old_name = v.name ;
                vm.binding = v.binding ;
                vm.fq = v.fq ;
                mappings.emplace_back( std::move( vm ) ) ;
            }
        }
    }

    // determine geometry stage vertex inputs/outputs
    {
        // last shader in geometry stage
        natus::nsl::shader_type last_shader = natus::nsl::shader_type::vertex_shader ; 

        for( auto const & var : mappings )
        {
            if( var.fq != natus::nsl::flow_qualifier::out ) continue ;
            
            if( var.st == natus::nsl::shader_type::geometry_shader ) 
                last_shader = natus::nsl::shader_type::geometry_shader ;
        }

        // vertex attributes are vertex shader inputs
        for( auto const & vm : mappings )
        {
            natus::nsl::generated_code_t::variable_t var =
            {
                vm.new_name,
                vm.binding, 
                vm.fq
            } ;

            // determine inputs
            if( vm.fq == natus::nsl::flow_qualifier::in &&
                vm.st == natus::nsl::shader_type::vertex_shader )
            {
                ret.geometry_ins.emplace_back( var ) ;
            }

            // determine outputs
            else if( vm.fq == natus::nsl::flow_qualifier::out &&
                    vm.st == last_shader )
            {
                var.name = "streamout." + var.name ; // must be streamout
                ret.geometry_outs.emplace_back( var ) ;
            }
        }
    }

    // streamout type
    {
        bool_t has_pixel_shader = false ;

        for( auto const& s : _genable.config.shaders )
        {
            if( s.type == natus::nsl::shader_type::pixel_shader )
            {
                has_pixel_shader = true ;
                break ;
            }
            
        }

        if( !has_pixel_shader ) ret.streamout = natus::nsl::streamout_type::interleaved ; 
    }

    natus::nsl::generated_code_t::shaders_t shaders ;
    // glsl 
    {
        auto const shds = natus::nsl::glsl::generator_t().generate( _genable, mappings ) ;
        for( auto shd_ : shds )
        {
            shaders.emplace_back( shd_ ) ;
        }
    }

    // hlsl
    {
        auto const shds = natus::nsl::hlsl::generator_t().generate( _genable, mappings ) ;
        for( auto shd_: shds )
        {
            shaders.emplace_back( shd_ ) ;
        }
    }
    ret.shaders = std::move( shaders ) ;

    return std::move( ret ) ;
}