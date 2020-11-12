
#pragma once

#include "../typedefs.h"
#include "../object/shader_object.h"
#include <natus/nsl/generator_structs.hpp>

namespace natus
{
    namespace graphics
    {
        // generates a shader configuration from generated code
        class nsl_bridge
        {
            natus_this_typedefs( nsl_bridge ) ;

        public:

            natus::graphics::shader_object_t create( natus::nsl::generated_code_cref_t code ) const noexcept
            {
                natus::graphics::shader_object_t ret ;

                // variable bindings
                {
                    for( auto const& s : code.shaders )
                    {
                        // vertex attributes
                        {
                            for( auto const& v : s.variables )
                            {
                                if( s.type != natus::nsl::shader_type::vertex_shader ) continue ;

                                // check texcoord
                                {
                                    if( natus::nsl::is_texcoord( v.binding ) )
                                    {
                                        
                                        natus::graphics::vertex_attribute va
                                            = natus::graphics::vertex_attribute::undefined ;

                                        switch( v.binding )
                                        {
                                        case natus::nsl::binding::texcoord0: 
                                            va = natus::graphics::vertex_attribute::texcoord0 ; break ;
                                        case natus::nsl::binding::texcoord1: 
                                            va = natus::graphics::vertex_attribute::texcoord1 ; break ;
                                        case natus::nsl::binding::texcoord2: 
                                            va = natus::graphics::vertex_attribute::texcoord2 ; break ;
                                        case natus::nsl::binding::texcoord3: 
                                            va = natus::graphics::vertex_attribute::texcoord3 ; break ;
                                        case natus::nsl::binding::texcoord4: 
                                            va = natus::graphics::vertex_attribute::texcoord4 ; break ;
                                        case natus::nsl::binding::texcoord5: 
                                            va = natus::graphics::vertex_attribute::texcoord5 ; break ;
                                        case natus::nsl::binding::texcoord6: 
                                            va = natus::graphics::vertex_attribute::texcoord6 ; break ;
                                        case natus::nsl::binding::texcoord7: 
                                            va = natus::graphics::vertex_attribute::texcoord7 ; break ;
                                        default: break;
                                        }
                                        ret.add_vertex_input_binding( va, v.name ) ;

                                        continue ;
                                    }
                                }

                                // check color
                                {
                                    if( natus::nsl::is_color( v.binding ) )
                                    {
                                        natus::graphics::vertex_attribute va
                                            = natus::graphics::vertex_attribute::undefined ;

                                        switch( v.binding )
                                        {
                                        case natus::nsl::binding::color0: 
                                            va = natus::graphics::vertex_attribute::color0 ; break ;
                                        case natus::nsl::binding::color1: 
                                            va = natus::graphics::vertex_attribute::color1 ; break ;
                                        case natus::nsl::binding::color2: 
                                            va = natus::graphics::vertex_attribute::color2 ; break ;
                                        case natus::nsl::binding::color3: 
                                            va = natus::graphics::vertex_attribute::color3 ; break ;
                                        case natus::nsl::binding::color4: 
                                            va = natus::graphics::vertex_attribute::color4 ; break ;
                                        case natus::nsl::binding::color5: 
                                            va = natus::graphics::vertex_attribute::color5 ; break ;
                                        default: break;
                                        }
                                        ret.add_vertex_input_binding( va, v.name ) ;
                                        continue ;
                                    }
                                }

                                if( v.binding == natus::nsl::binding::position )
                                {
                                    ret.add_vertex_input_binding( natus::graphics::vertex_attribute::position, v.name ) ;
                                }
                                else if( v.binding == natus::nsl::binding::normal )
                                {
                                    ret.add_vertex_input_binding( natus::graphics::vertex_attribute::normal, v.name ) ;
                                }
                                else if( v.binding == natus::nsl::binding::tangent )
                                {
                                    ret.add_vertex_input_binding( natus::graphics::vertex_attribute::tangent, v.name ) ;
                                }
                            }
                        }


                        {
                            for( auto const& v : s.variables )
                            {
                                natus::graphics::binding_point bp = natus::graphics::binding_point::undefined ;
                                
                                if( v.binding == natus::nsl::binding::object )
                                {
                                    bp = natus::graphics::binding_point::object_matrix ;
                                }
                                else if( v.binding == natus::nsl::binding::world )
                                {
                                    bp = natus::graphics::binding_point::world_matrix ;
                                }
                                else if( v.binding == natus::nsl::binding::view )
                                {
                                    bp = natus::graphics::binding_point::view_matrix ;
                                }
                                else if( v.binding == natus::nsl::binding::projection )
                                {
                                    bp = natus::graphics::binding_point::projection_matrix ;
                                }
                                else if( v.binding == natus::nsl::binding::camera )
                                {
                                    bp = natus::graphics::binding_point::camera_matrix ;
                                }
                                else if( v.binding == natus::nsl::binding::camera_position )
                                {
                                    bp = natus::graphics::binding_point::camera_position ;
                                }
                                else if( v.binding == natus::nsl::binding::viewport )
                                {
                                    bp = natus::graphics::binding_point::viewport_dimension ;
                                }

                                if( !ret.has_input_binding( bp ) && bp != natus::graphics::binding_point::undefined ) 
                                    ret.add_input_binding( bp, v.name ) ;
                            }
                        }
                    }
                }
                
                // code
                {
                    natus::ntd::vector< natus::nsl::api_type > const types = 
                    {
                        natus::nsl::api_type::es3, natus::nsl::api_type::gl3,
                        natus::nsl::api_type::d3d11 
                    } ;

                    for( auto const & t : types )
                    {
                        natus::graphics::shader_set_t ss ;

                        natus::graphics::backend_type bt = natus::graphics::backend_type::unknown ;
                        switch( t )
                        {
                        case natus::nsl::api_type::gl3:
                            bt = natus::graphics::backend_type::gl3 ;
                            break ;
                        case natus::nsl::api_type::es3:
                            bt = natus::graphics::backend_type::es3 ;
                            break ;
                        case natus::nsl::api_type::d3d11:
                            bt = natus::graphics::backend_type::d3d11 ;
                            break ;
                        default:
                            break;
                        }

                        if( bt == natus::graphics::backend_type::unknown )
                        {
                            natus::log::global_t::warning( "[nsl_bridge] : unknown/unmappable api type" ) ;
                            continue ;
                        }

                        code.sorted_by_api_type( t, [&] ( natus::nsl::shader_type st, natus::nsl::generated_code_t::code_cref_t c )
                        {
                            if( st == natus::nsl::shader_type::vertex_shader )
                                ss.set_vertex_shader( c.shader ) ;
                            else if( st == natus::nsl::shader_type::pixel_shader )
                                ss.set_pixel_shader( c.shader ) ;
                        } ) ;

                        ret.insert( bt, std::move( ss ) ) ;
                    }
                }


                return std::move( ret ) ;
            }

        };
        natus_typedef( nsl_bridge ) ;
    }
}