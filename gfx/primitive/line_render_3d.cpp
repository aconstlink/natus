
#include "line_render_3d.h"

#include <natus/concurrent/parallel_for.hpp>

using namespace natus::gfx ;

//**********************************************************
line_render_3d::line_render_3d( void_t ) noexcept 
{
}
            
//**********************************************************
line_render_3d::line_render_3d( this_rref_t ) noexcept 
{
}
            
//**********************************************************
line_render_3d::~line_render_3d( void_t ) noexcept 
{
}
            
//**********************************************************
void_t line_render_3d::init( natus::ntd::string_cref_t name, natus::graphics::async_views_t asyncs ) noexcept 
{
    _name = name ;
    _asyncs = asyncs ;

    // root render states
    {
        natus::graphics::state_object_t so = natus::graphics::state_object_t(
            name + ".render_states" ) ;

        {
            natus::graphics::render_state_sets_t rss ;

            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = false ;
            rss.depth_s.ss.do_depth_write = false ;

            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = false ;
            rss.polygon_s.ss.ff = natus::graphics::front_face::clock_wise ;
            rss.polygon_s.ss.cm = natus::graphics::cull_mode::back ;
            rss.polygon_s.ss.fm = natus::graphics::fill_mode::fill ;

            rss.blend_s.do_change = true ;
            rss.blend_s.ss.do_activate = true ;
            rss.blend_s.ss.src_blend_factor = natus::graphics::blend_factor::src_alpha ;
            rss.blend_s.ss.dst_blend_factor = natus::graphics::blend_factor::one_minus_src_alpha ;
                   
            so.add_render_state_set( rss ) ;
        }

        _rs = std::move( so ) ;
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( _rs ) ;
        } ) ;
    }

    // geometry configuration
    {
        auto vb = natus::graphics::vertex_buffer_t()
            .add_layout_element( natus::graphics::vertex_attribute::position, natus::graphics::type::tfloat, natus::graphics::type_struct::vec3 )
            .resize( 4 ).update<this_t::vertex>( [=] ( this_t::vertex* array, size_t const ne )
        {
            array[ 0 ].pos = natus::math::vec3f_t( -0.5f, -0.5f ) ;
            array[ 1 ].pos = natus::math::vec3f_t( -0.5f, +0.5f ) ;
            array[ 2 ].pos = natus::math::vec3f_t( +0.5f, +0.5f ) ;
            array[ 3 ].pos = natus::math::vec3f_t( +0.5f, -0.5f ) ;
        } );

        auto ib = natus::graphics::index_buffer_t() ;

        natus::graphics::geometry_object_res_t geo = natus::graphics::geometry_object_t( name + ".lines",
            natus::graphics::primitive_type::lines, std::move( vb ), std::move( ib ) ) ;

        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( geo ) ;
        } ) ;                
        _go = std::move( geo ) ;
    }

    // array
    {
        natus::graphics::data_buffer_t db = natus::graphics::data_buffer_t()
            .add_layout_element( natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 )
            .resize( 2 ).update< the_data >( [&]( the_data * array, size_t const ne )
            {
                for( size_t i=0; i<ne; ++i )
                {
                    array[i].color = natus::math::vec4f_t ( 0.0f, 0.5f, 1.0f, 1.0f) ;
                }
            });

        _ao = natus::graphics::array_object_t( name + ".per_line_data", std::move( db ) ) ;
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( _ao ) ;
        } ) ;
    }

    // shader configuration
    {
        natus::graphics::shader_object_t sc( name + ".shader" ) ;

        // shaders : ogl 3.1
        {
            natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 140

                    in vec3 in_pos ;
                    out vec4 var_col ;
                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;
                    uniform mat4 u_world ;
                    uniform samplerBuffer u_data ;
                    uniform int u_offset ;

                    void main()
                    {
                        int idx = gl_VertexID / 2 ;
                        vec4 color = texelFetch( u_data, idx ) ;

                        var_col = color ;
                        vec4 pos = vec4( in_pos, 1.0 )  ;
                        gl_Position = u_proj * u_view * u_world * pos ;

                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 140

                    in vec4 var_col ;
                    out vec4 out_color ;

                    void main()
                    {    
                        out_color = var_col ;
                    } )" ) ) ;

            sc.insert( natus::graphics::shader_api_type::glsl_1_4, std::move( ss ) ) ;
        }

        // shaders : es 3.0
        {
            natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 300 es

                    in vec3 in_pos ;
                    out vec4 var_col ;
                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;
                    uniform mat4 u_world ;
                    uniform sampler2D u_data ;
                    uniform int u_offset ;

                    void main()
                    {
                        int idx = gl_VertexID / 2 ;
                        ivec2 wh = textureSize( u_data, 0 ) ;
                        var_col = texelFetch( u_data, ivec2( ((idx) % wh.x), (idx / wh.x) ), 0 ) ;

                        vec4 pos = vec4( in_pos, 1.0 )  ;
                        gl_Position = u_proj * u_view * u_world * pos ;
                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 300 es
                    precision mediump float ;
                    in vec4 var_col ;
                    layout(location = 0 ) out vec4 out_color ;

                    void main()
                    {    
                        out_color = var_col ;
                    })" ) ) ;

            sc.insert( natus::graphics::shader_api_type::glsles_3_0, std::move( ss ) ) ;
        }

        // shaders : hlsl 11(5.0)
        {
            natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                    cbuffer ConstantBuffer : register( b0 ) 
                    {
                        float4x4 u_proj ;
                        float4x4 u_view ;
                        float4x4 u_world ;
                    }

                    struct VS_INPUT
                    {
                        uint in_id: SV_VertexID ;
                        float3 in_pos : POSITION ; 
                    } ;
                    struct VS_OUTPUT
                    {
                        float4 pos : SV_POSITION;
                        float4 col : COLOR0;
                    };
                            
                    Buffer< float4 > u_data ;

                    VS_OUTPUT VS( VS_INPUT input )
                    {
                        VS_OUTPUT output = (VS_OUTPUT)0 ;
                        int idx = input.in_id / 2 ;
                        float4 col = u_data.Load( idx ) ;
                        output.col = col ;
                        float4 pos = float4( input.in_pos, 1.0f )  ;
                        output.pos = mul( pos, u_world ) ;
                        output.pos = mul( output.pos, u_view ) ;
                        output.pos = mul( output.pos, u_proj ) ;
                        return output;
                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    
                    struct VS_OUTPUT
                    {
                        float4 Pos : SV_POSITION;
                        float4 col : COLOR0;
                    };

                    float4 PS( VS_OUTPUT input ) : SV_Target0
                    {
                        return input.col ;
                    } )" ) ) ;

            sc.insert( natus::graphics::shader_api_type::hlsl_5_0, std::move( ss ) ) ;
        }

        // configure more details
        {
            sc
                .add_vertex_input_binding( natus::graphics::vertex_attribute::position, "in_pos" )
                .add_input_binding( natus::graphics::binding_point::view_matrix, "u_view" )
                .add_input_binding( natus::graphics::binding_point::projection_matrix, "u_proj" ) ;
        }

        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( sc ) ;
        } ) ;

        _so = std::move( sc ) ;
    }

    // the render object
    {
        natus::graphics::render_object_t rc = natus::graphics::render_object_t( name + ".render_object" ) ;

        {
            rc.link_geometry( name + ".lines" ) ;
            rc.link_shader( name + ".shader" ) ;
        }

        // add variable set 
        {
           this_t::add_variable_set( rc ) ;
        }
                
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( rc ) ;
        } ) ;
        _ro = std::move( rc ) ;
    }
}

//**********************************************************
void_t line_render_3d::release( void_t ) noexcept 
{
}

//**********************************************************
void_t line_render_3d::draw( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec4f_cref_t color ) noexcept
{
    this_t::line_t ln ;
    ln.points.p0 = p0 ;
    ln.points.p1 = p1 ;
    ln.color = color ;

    {
        natus::concurrent::lock_guard_t lk( _lines_mtx ) ;
        if( _lines.capacity() == _lines.size() )
            _lines.reserve( _lines.size() + 100 ) ;
        
        _lines.emplace_back( std::move( ln ) ) ;
    }
}

//**********************************************************
void_t line_render_3d::prepare_for_rendering( void_t ) noexcept 
{
    bool_t vertex_realloc = false ;
    bool_t data_realloc = false ;
    bool_t reconfig_ro = false ;

    // 1. copy data
    {
        size_t const vsib = _go->vertex_buffer().get_sib() ;
        size_t const bsib = _ao->data_buffer().get_sib() ;

        _go->vertex_buffer().resize( _lines.size() << 1 ) ;
        _ao->data_buffer().resize( _lines.size() ) ;

        // copy vertices
        {
            size_t const num_verts = _lines.size() << 1 ;
            _go->vertex_buffer().update<this_t::vertex>( 0, num_verts, 
                [&]( this_t::vertex * array, size_t const ne )
            {
                natus::concurrent::parallel_for<size_t>( natus::concurrent::range_1d<size_t>(0, ne>>1),
                    [&]( natus::concurrent::range_1d<size_t> const & r )
                {
                    for( size_t l=r.begin(); l<r.end(); ++l )
                    {
                        size_t const w = l << 1 ;
                        array[ w + 0 ].pos = _lines[ l ].pa[0] ;
                        array[ w + 1 ].pos = _lines[ l ].pa[1] ;
                    }
                } ) ;
            } ) ;
        }

        // copy data
        {
            natus::concurrent::parallel_for<size_t>( natus::concurrent::range_1d<size_t>(0, _lines.size()),
                [&]( natus::concurrent::range_1d<size_t> const & r )
            {
                for( size_t l=r.begin(); l<r.end(); ++l )
                {
                    size_t const idx = l ;
                    _ao->data_buffer().update< natus::math::vec4f_t >( idx, _lines[l].color ) ;
                }
            } ) ;
        }

        vertex_realloc = _go->vertex_buffer().get_sib() > vsib ;
        data_realloc = _ao->data_buffer().get_sib() > bsib ;

        _num_lines = _lines.size() ;
        _lines.clear() ;
    }

    // 2. prepare variable set
    {
        _ro->for_each( [&]( size_t const i, natus::graphics::variable_set_res_t const & vars )
        {
            {
                auto* var = vars->data_variable<natus::math::mat4f_t>( "u_view" ) ;
                var->set( _view ) ;
            }
            {
                auto* var = vars->data_variable<natus::math::mat4f_t>( "u_proj" ) ;
                var->set( _proj ) ;
            }
        } ) ;
    }

    // 3. tell the graphics api
    {
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        { 
            if( reconfig_ro ) a.configure( _ro ) ;

            if( vertex_realloc ) a.configure( _go ) ;
            else a.update( _go ) ;

            if( data_realloc ) a.configure( _ao ) ;
            else a.update( _ao ) ;
        } ) ;
    }
}

//**********************************************************
void_t line_render_3d::render( void_t ) noexcept 
{
    _asyncs.for_each( [&]( natus::graphics::async_view_t a )
    {
        a.push( _rs ) ;
        {
            natus::graphics::backend::render_detail rd ;
            rd.num_elems = _num_lines << 1 ;
            rd.start = 0 ;
            rd.varset = 0 ;
            a.render( _ro, rd ) ;
        }
        a.pop( natus::graphics::backend::pop_type::render_state ) ;
        
    } ) ;
}
            
//**********************************************************
void_t line_render_3d::add_variable_set( natus::graphics::render_object_ref_t rc ) noexcept 
{
    natus::graphics::variable_set_res_t vars = natus::graphics::variable_set_t() ;
            
    {
        auto* var = vars->array_variable( "u_data" ) ;
        var->set( _name + ".per_line_data" ) ;
    }
    {
        auto* var = vars->data_variable<int32_t>( "u_offset" ) ;
        var->set( 0 ) ;
    }
    {
        auto* var = vars->data_variable<natus::math::mat4f_t>( "u_world" ) ;
        var->set( natus::math::mat4f_t().identity() ) ;
    }
    {
        auto* var = vars->data_variable<natus::math::mat4f_t>( "u_view" ) ;
        var->set( natus::math::mat4f_t().identity() ) ;
    }
    {
        auto* var = vars->data_variable<natus::math::mat4f_t>( "u_proj" ) ;
        var->set( natus::math::mat4f_t().identity() ) ;
    }

    rc.add_variable_set( std::move( vars ) ) ;
}

//**********************************************************
void_t line_render_3d::set_view_proj( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) noexcept 
{
    _view = view ;
    _proj = proj ;
}