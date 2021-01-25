
#include "sprite_render_2d.h"

#include <natus/math/utility/constants.hpp>

using namespace natus::gfx ;

sprite_render_2d::sprite_render_2d( void_t ) 
{
    _rs = natus::graphics::state_object_t() ;
    _ao = natus::graphics::array_object_t() ;
    _ro = natus::graphics::render_object_t() ;
    _go = natus::graphics::geometry_object_t() ;
    _so = natus::graphics::shader_object_t() ;
}

sprite_render_2d::sprite_render_2d( this_rref_t rhv ) 
{
    _rs = std::move( rhv._rs ) ;
    _ao = std::move( rhv._ao ) ;
    _ro = std::move( rhv._ro ) ;
    _go = std::move( rhv._go ) ;
    _so = std::move( rhv._so ) ;
}
            
sprite_render_2d::~sprite_render_2d( void_t ) 
{
}

void_t sprite_render_2d::init( natus::ntd::string_cref_t name, natus::graphics::async_views_t asyncs ) noexcept 
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
            .resize( _max_quads * 4 ).update<this_t::vertex>( [&] ( this_t::vertex* array, size_t const ne )
        {
            for( size_t i=0; i<_max_quads; ++i )
            {
                size_t const base = i << 2 ;
                array[ base + 0 ].pos = natus::math::vec2f_t( -0.5f, -0.5f ) ;
                array[ base + 1 ].pos = natus::math::vec2f_t( -0.5f, +0.5f ) ;
                array[ base + 2 ].pos = natus::math::vec2f_t( +0.5f, +0.5f ) ;
                array[ base + 3 ].pos = natus::math::vec2f_t( +0.5f, -0.5f ) ;
            }
            
        } );

        auto ib = natus::graphics::index_buffer_t().
            set_layout_element( natus::graphics::type::tuint ).resize( _max_quads * 6 ).
            update<uint_t>( [&] ( uint_t* array, size_t const ne )
        {
            for( size_t i=0; i<_max_quads; ++i )
            {
                size_t const bi = i * 6 ;
                size_t const bv = i * 4 ;

                array[ bi + 0 ] = bv + 0 ;
                array[ bi + 1 ] = bv + 1 ;
                array[ bi + 2 ] = bv + 2 ;

                array[ bi + 3 ] = bv + 0 ;
                array[ bi + 4 ] = bv + 2 ;
                array[ bi + 5 ] = bv + 3 ;
            }

            
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
            .resize( 2 ) ;

        _ao = natus::graphics::array_object_t( name + ".per_sprite_data", std::move( db ) ) ;
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
                    uniform int u_offset ;
                    uniform samplerBuffer u_data ;

                    void main()
                    {
                        int idx = (gl_VertexID / 4) * 3 + u_offset * 3 ;
                        vec4 d0 = texelFetch( u_data, idx + 0 ) ; // pos scale
                        vec4 d1 = texelFetch( u_data, idx + 1 ) ; // frame
                        vec4 d2 = texelFetch( u_data, idx + 2 ) ; // uv rect

                        mat2 scaling = mat2( d0.z, 0.0, 0.0, d0.w ) ;
                        mat2 frame = mat2( d1.xy, d1.zw ) ;

                        var_col = vec4( d2.zw, 0.0, 1.0 ) ;
                        vec4 pos = vec4( d0.xy + frame * scaling * in_pos, 0.0, 1.0 )  ;
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
                        int u_offset ;
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
                        int idx = (input.in_id / 4) * 3 + u_offset * 3 ;
                        float4 d0 = u_data.Load( idx + 0 ) ;
                        float4 d1 = u_data.Load( idx + 1 ) ;
                        float4 d2 = u_data.Load( idx + 2 ) ;

                        output.col = d2 ;
                        float4 pos = float4( d0.xy + input.in_pos * d0.zw, 0.0f, 1.0f )  ;
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
            this_t::add_variable_set( rc ) ;
        }
                
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( rc ) ;
        } ) ;
        _ro = std::move( rc ) ;
    }
}

void_t sprite_render_2d::release( void_t ) noexcept 
{
}

size_t sprite_render_2d::get_slot( natus::ntd::string_cref_t name ) noexcept 
{
    return 0 ;
}

size_t sprite_render_2d::add_texture( natus::ntd::string_cref_t name ) noexcept 
{
    return 0 ;
}

void_t sprite_render_2d::draw( size_t const l, natus::math::vec2f_cref_t pos, natus::math::mat2f_cref_t frame, natus::math::vec2f_cref_t scale, natus::math::vec4f_cref_t uv_rect, size_t const slot ) noexcept 
{
    if( _layers.size() <= l+1 ) 
    {
        _layers.resize( l+1 ) ;
    }

    this_t::sprite_t d ;
    d.pos = pos ;
    d.frame = frame ;
    d.scale = scale ;
    d.uv_rect = uv_rect ;
    d.slot = slot ;

    _layers[l].sprites.emplace_back( std::move( d ) ) ;
    ++_num_sprites ;
}
            
void_t sprite_render_2d::prepare_for_rendering( void_t ) noexcept 
{
    bool_t vertex_realloc = false ;
    bool_t data_realloc = false ;
    bool_t more_varsets = false ;

    if( _num_sprites == 0 ) return ;

    // 1. prepare render data
    {
        size_t const num_vs = (_num_sprites / _max_quads) + _layers.size() ;
        _render_data.resize( num_vs ) ;
        _render_layer_infos.resize( _layers.size() ) ;

        size_t rd_idx = 0 ;
        
        size_t rd_start = 0 ;

        for( size_t i=0; i<_layers.size(); ++i )
        {
            auto const & sprites = _layers[i].sprites ;
            
            // number of render calls for this layer
            size_t const num_render = sprites.size() / _max_quads ; 

            for( size_t r=0; r< num_render; ++r, ++rd_idx )
            {
                _render_data[rd_idx].num_quads = _max_quads ;
                _render_data[rd_idx].num_elems = _max_quads * 6 ;
            }

            size_t const residuals = sprites.size() % _max_quads ;
            if( residuals != 0 )
            {
                _render_data[rd_idx].num_quads = residuals ;
                _render_data[rd_idx].num_elems = residuals * 6 ;
                ++rd_idx ;
            }

            this_t::render_layer_info rli = { rd_start, rd_idx } ;
            _render_layer_infos[i] = std::move( rli ) ;

            rd_start = rd_idx ;
        }
    }

    // 2. prepare variable sets
    {
        for( size_t i=_ro->get_num_variable_sets(); i<_render_data.size(); ++i )
        {
            this_t::add_variable_set( *_ro ) ;
            more_varsets = true ;
        }

        int_t offset = 0 ;
        for( size_t i=0; i<_render_data.size(); ++i )
        {
            _ro->get_variable_set(i)->data_variable<int32_t>( "u_offset" )->set( offset ) ;
            offset += int32_t( _render_data[i].num_quads ) ;
        }
    }

    // 3. copy data
    {
        size_t const bsib = _ao->data_buffer().get_sib() ;

        size_t const sizeof_data = sizeof(this_t::the_data) / sizeof( natus::math::vec4f_t ) ;
        _ao->data_buffer().resize( _num_sprites * sizeof_data ) ;

        size_t lstart = 0 ;

        for( size_t i=0; i<_layers.size(); ++i )
        {
            auto const & sprites = _layers[i].sprites ;
            
            for( size_t i=0; i<sprites.size(); ++i )
            {
                size_t const idx = lstart + i * sizeof_data ;
                _ao->data_buffer().update< natus::math::vec4f_t >( idx + 0, 
                    natus::math::vec4f_t( sprites[i].pos, sprites[i].scale ) ) ;

                _ao->data_buffer().update< natus::math::vec4f_t >( idx + 1, 
                    natus::math::vec4f_t( sprites[i].frame.column(0), sprites[i].frame.column(1) ) ) ;

                _ao->data_buffer().update< natus::math::vec4f_t >( idx + 2, 
                    sprites[i].uv_rect ) ;

                //_ao->data_buffer().update< natus::math::vec4f_t >( idx + 3, 
                    //  natus::math::vec4f_t( sprites[i].pos, sprites[i].scale ) ) ;
            }
            lstart += sprites.size() * sizeof_data ;
            
            _layers[i].sprites.clear() ;
        }
        
        data_realloc = _ao->data_buffer().get_sib() > bsib ;
    }

    // 4. tell the graphics api
    {
        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        { 
            if( data_realloc ) a.configure( _ao ) ;
            else a.update( _ao ) ;

            if( more_varsets ) a.configure( _ro ) ;
        } ) ;
    }

    _num_sprites = 0 ;
}
            
void_t sprite_render_2d::render( size_t const l ) noexcept 
{
    _asyncs.for_each( [&]( natus::graphics::async_view_t a )
    { 
        a.use( _rs ) ;
        
        if( _render_layer_infos.size() > l ) 
        {
            auto const & rli = _render_layer_infos[l] ;

            for( size_t idx= rli.start; idx < rli.end; ++idx )
            {
                auto const & plrd = _render_data[idx] ;

                natus::graphics::backend::render_detail rd ;
                rd.start = 0 ;
                rd.num_elems = plrd.num_elems ;
                rd.varset = idx ;

                a.render( _ro, rd ) ;
            }
        }
        
        a.use( natus::graphics::state_object_t() ) ;
    } ) ;
}

void_t sprite_render_2d::add_variable_set( natus::graphics::render_object_ref_t rc ) noexcept 
{
    natus::graphics::variable_set_res_t vars = natus::graphics::variable_set_t() ;
            
    {
        auto* var = vars->array_variable( "u_data" ) ;
        var->set( _name + ".per_sprite_data" ) ;
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