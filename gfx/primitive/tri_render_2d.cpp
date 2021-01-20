
#include "tri_render_2d.h"

#include <natus/math/utility/constants.hpp>

using namespace natus::gfx ;

tri_render_2d::tri_render_2d( void_t ) 
{
    _rs = natus::graphics::state_object_t() ;
    _ao = natus::graphics::array_object_t() ;
    _ro = natus::graphics::render_object_t() ;
    _go = natus::graphics::geometry_object_t() ;
    _so = natus::graphics::shader_object_t() ;
}

tri_render_2d::tri_render_2d( this_rref_t rhv ) 
{
    _rs = std::move( rhv._rs ) ;
    _ao = std::move( rhv._ao ) ;
    _ro = std::move( rhv._ro ) ;
    _go = std::move( rhv._go ) ;
    _so = std::move( rhv._so ) ;
}
            
tri_render_2d::~tri_render_2d( void_t ) 
{
}

void_t tri_render_2d::init( natus::ntd::string_cref_t name, natus::graphics::async_views_t asyncs ) noexcept 
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
            .add_layout_element( natus::graphics::vertex_attribute::position, natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 )
            .resize( 3 ).update<this_t::vertex>( [=] ( this_t::vertex* array, size_t const ne )
        {
            array[ 0 ].pos = natus::math::vec2f_t( -0.5f, -0.5f ) ;
            array[ 1 ].pos = natus::math::vec2f_t( -0.5f, +0.5f ) ;
            array[ 2 ].pos = natus::math::vec2f_t( +0.5f, +0.5f ) ;
        } );

        auto ib = natus::graphics::index_buffer_t().
            set_layout_element( natus::graphics::type::tuint ).resize( 3 ).
            update<uint_t>( [] ( uint_t* array, size_t const ne )
        {
            array[ 0 ] = 0 ;
            array[ 1 ] = 1 ;
            array[ 2 ] = 2 ;
        } ) ;

        natus::graphics::geometry_object_res_t geo = natus::graphics::geometry_object_t( name + ".geometry",
            natus::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ) ;

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

        _ao = natus::graphics::array_object_t( name + ".per_tri_data", std::move( db ) ) ;
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

                    in vec2 in_pos ;
                    out vec4 var_col ;
                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;
                    uniform mat4 u_world ;
                    uniform samplerBuffer u_data ;

                    void main()
                    {
                        int idx = gl_VertexID / 3 ;
                        vec4 color = texelFetch( u_data, idx ) ;
                        
                        var_col = color ;
                        vec4 pos = vec4( in_pos, 0.0, 1.0 )  ;
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

            sc.insert( natus::graphics::backend_type::gl3, std::move( ss ) ) ;
        }

        // shaders : es 3.0
        {
            natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 300 es

                    in vec2 in_pos ;
                    out vec4 var_col ;
                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;
                    uniform mat4 u_world ;
                    uniform sampler2D u_data ;

                    void main()
                    {
                        int idx = gl_VertexID / 3 ;
                        ivec2 wh = textureSize( u_data, 0 ) ;
                        var_col = texelFetch( u_data, ivec2( ((idx*3) % wh.x), (idx / wh.x) ), 0 ) ;
                        
                        vec4 pos = vec4( in_pos, 0.0, 1.0 )  ;
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

            sc.insert( natus::graphics::backend_type::es3, std::move( ss ) ) ;
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
                        float2 in_pos : POSITION ; 
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
                        int idx = input.in_id / 3 ;
                        float4 col = u_data.Load( idx ) ;
                        output.col = col ;
                        float4 pos = float4( input.in_pos, 0.0f, 1.0f )  ;
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

            sc.insert( natus::graphics::backend_type::d3d11, std::move( ss ) ) ;
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
            rc.link_geometry( name + ".geometry" ) ;
            rc.link_shader( name + ".shader" ) ;
        }

        // add variable set 
        {
            natus::graphics::variable_set_res_t vars = natus::graphics::variable_set_t() ;
            
            {
                auto* var = vars->array_variable( "u_data" ) ;
                var->set( name + ".per_tri_data" ) ;
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
                
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( rc ) ;
        } ) ;
        _ro = std::move( rc ) ;
    }
}

void_t tri_render_2d::release( void_t ) noexcept 
{
}

void_t tri_render_2d::draw( size_t const l, natus::math::vec2f_cref_t p0, natus::math::vec2f_cref_t p1,
    natus::math::vec2f_cref_t p2, natus::math::vec4f_cref_t color ) noexcept
{
    if( _layers.size() <= l+1 ) 
    {
        _layers.resize( l+1 ) ;
        _render_data.resize( l+1 ) ;
    }

    this_t::tri_t ln ;
    ln.pts.p0 = p0 ;
    ln.pts.p1 = p1 ;
    ln.pts.p2 = p2 ;
    ln.color = color ;

    _layers[l].tris.emplace_back( std::move( ln ) ) ;
    ++_num_tris ;
}

void_t tri_render_2d::draw_rect( size_t const l, 
    natus::math::vec2f_cref_t p0, natus::math::vec2f_cref_t p1, 
    natus::math::vec2f_cref_t p2, natus::math::vec2f_cref_t p3, natus::math::vec4f_cref_t color ) noexcept 
{
    this_t::draw( l, p0, p1, p2, color ) ;
    this_t::draw( l, p0, p2, p3, color ) ;
}

void_t tri_render_2d::draw_circle( size_t const l, size_t const s, natus::math::vec2f_cref_t p0, float_t const r, natus::math::vec4f_cref_t color ) noexcept 
{
    size_t const segs = std::max( size_t(5), s ) ;
    natus::ntd::vector< natus::math::vec2f_t > points( segs + 1 ) ;

    points[0] = p0 ;

    float_t a = 0.0f ;
    float_t const del = 2.0f * natus::math::constants<float_t>::pi() / float_t(segs) ;
    for( size_t i=0; i<segs; ++i )
    {
        points[i] = p0 + natus::math::vec2f_t( std::cos( a ), std::sin( a ) ) * 
            natus::math::vec2f_t(r) ;

        a += del ;
    }

    for( size_t i=0; i<segs-1; ++i )
    {
        this_t::draw( l, points[0], points[i], points[i+1], color ) ;
    }
}

            
void_t tri_render_2d::prepare_for_rendering( void_t ) noexcept 
{
    bool_t vertex_realloc = false ;
    bool_t data_realloc = false ;

    // 1. copy data
    {
        size_t const vsib = _go->vertex_buffer().get_sib() ;
        size_t const bsib = _ao->data_buffer().get_sib() ;

        _go->vertex_buffer().resize( _num_tris * 3 ) ;
        _go->index_buffer().resize( _num_tris *3 ) ;
        _ao->data_buffer().resize( _num_tris ) ;

        size_t start = 0 ;
        size_t lstart = 0 ;

        size_t istart = 0 ;

        for( size_t i=0; i<_layers.size(); ++i )
        {
            auto const & tris = _layers[i].tris ;

            // copy vertices
            {
                size_t const num_verts = tris.size() * 3 ;
                _go->vertex_buffer().update<this_t::vertex>( start, start+num_verts, 
                    [&]( this_t::vertex * array, size_t const ne )
                {
                    for( size_t i=0; i<ne; ++i )
                    {
                        size_t const ls = i / 3 ;
                        array[i].pos = tris[ls].array[ i % 3 ] ;
                    }
                } ) ;
                start += num_verts ;
                
                _go->index_buffer().update<uint32_t>( [&]( uint32_t * array, size_t const ne )
                {
                    for( size_t i=0; i<ne; ++i )
                    {
                        array[i] = i ;
                    }
                } ) ;
            }
        
            // copy color data
            {
                for( size_t i=0; i<tris.size();++i)
                {
                    size_t const idx = lstart + i ;
                    _ao->data_buffer().update< natus::math::vec4f_t >( idx, tris[i].color ) ;
                }
                lstart += tris.size() ;
            }

            _render_data[i].start = istart ;
            _render_data[i].num_elems = tris.size() * 3 ;
            istart = _render_data[i].num_elems ;
            _layers[i].tris.clear() ;
        }
        _num_tris = 0 ;

        vertex_realloc = _go->vertex_buffer().get_sib() > vsib ;
        data_realloc = _ao->data_buffer().get_sib() > bsib ;
    }

    // 2. tell the graphics api
    {
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        { 
            if( vertex_realloc ) a.configure( _go ) ;
            else a.update( _go ) ;

            if( data_realloc ) a.configure( _ao ) ;
            else a.update( _ao ) ;
        } ) ;
    }
}
            
void_t tri_render_2d::render( size_t const l ) noexcept 
{
    _asyncs.for_each( [&]( natus::graphics::async_view_t a )
    { 
        a.use( _rs ) ;
        
        if( l < _render_data.size() )
        {
            auto const & plrd = _render_data[l] ;
            natus::graphics::backend::render_detail rd ;
            rd.num_elems = plrd.num_elems ;
            rd.start = plrd.start ;
                
            a.render( _ro, rd ) ;
        }
        a.use( natus::graphics::state_object_t() ) ;
    } ) ;
}
            