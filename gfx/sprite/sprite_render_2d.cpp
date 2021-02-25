
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

void_t sprite_render_2d::init( natus::ntd::string_cref_t name, natus::ntd::string_cref_t image_name, natus::graphics::async_views_t asyncs ) noexcept 
{
    _name = name ;
    _asyncs = asyncs ;
    _image_name = image_name ;

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
            rss.polygon_s.ss.do_activate = true ;
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
            .resize( 5 ) ;

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
                    out vec3 var_uv ;
                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;
                    uniform mat4 u_world ;
                    uniform int u_offset ;
                    uniform samplerBuffer u_data ;

                    void main()
                    {
                        int idx = (gl_VertexID / 4) * 6 + u_offset * 6 ;
                        vec4 d0 = texelFetch( u_data, idx + 0 ) ; // pos scale
                        vec4 d1 = texelFetch( u_data, idx + 1 ) ; // frame
                        vec4 d2 = texelFetch( u_data, idx + 2 ) ; // uv rect
                        vec4 d3 = texelFetch( u_data, idx + 3 ) ; // additional
                        vec4 d4 = texelFetch( u_data, idx + 4 ) ; // uv animation
                        vec4 d5 = texelFetch( u_data, idx + 5 ) ; // color
                        
                        vec2 pivot = d3.xy * d0.zw * vec2( 0.5 ) ;
                        vec2 scale = d0.zw * (d2.zw - d2.xy) ;
                        vec2 pos_p = d0.xy + pivot ;

                        mat2 scaling = mat2( scale.x, 0.0, 0.0, scale.y ) ;
                        mat2 frame = mat2( d1.xy, d1.zw ) ;
    
                        vec2 uvs[4] = vec2[4]( d2.xy, d2.xw, d2.zw, d2.zy ) ;
                        var_uv.xy = uvs[gl_VertexID%4] ;
                        var_uv.z = d3.w ; // which texture layer

                        var_col = d5 ;
                        vec4 pos = vec4( pos_p + frame * scaling * in_pos, 0.0, 1.0 )  ;
                        gl_Position = u_proj * u_view * u_world * pos ;

                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 140

                    in vec4 var_col ;
                    in vec3 var_uv ;
                    out vec4 out_color ;
                    
                    uniform sampler2DArray u_tex ;

                    void main()
                    {    
                        out_color = texture( u_tex, var_uv ) * var_col ;
                    } )" ) ) ;

            sc.insert( natus::graphics::backend_type::gl3, std::move( ss ) ) ;
        }

        // shaders : es 3.0
        {
            natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 300 es
                    precision mediump float ;
                    layout( location = 0 ) in vec2 in_pos ;
                    out vec4 var_col ;
                    out vec3 var_uv ;
                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;
                    uniform mat4 u_world ;
                    uniform int u_offset ;
                    uniform sampler2D u_data ;

                    void main()
                    {
                        ivec2 wh = textureSize( u_data, 0 ) ;

                        int idx = (gl_VertexID / 4) * 6 + u_offset * 6 ;
                        vec4 d0 = texelFetch( u_data, ivec2((idx+0)%wh.x, (idx+0)/wh.x), 0 ) ; // pos scale
                        vec4 d1 = texelFetch( u_data, ivec2((idx+1)%wh.x, (idx+1)/wh.x), 0 ) ; // frame
                        vec4 d2 = texelFetch( u_data, ivec2((idx+2)%wh.x, (idx+2)/wh.x), 0 ) ; // uv rect
                        vec4 d3 = texelFetch( u_data, ivec2((idx+3)%wh.x, (idx+3)/wh.x), 0 ) ; // additional
                        vec4 d4 = texelFetch( u_data, ivec2((idx+4)%wh.x, (idx+4)/wh.x), 0 ) ; // uv animation
                        vec4 d5 = texelFetch( u_data, ivec2((idx+5)%wh.x, (idx+5)/wh.x), 0 ) ; // color

                        vec2 pivot = d3.xy * d0.zw * vec2( 0.5 ) ;
                        vec2 scale = d0.zw * (d2.zw - d2.xy) ;
                        vec2 pos_p = d0.xy + pivot ;

                        mat2 scaling = mat2( scale.x, 0.0, 0.0, scale.y ) ;
                        mat2 frame = mat2( d1.xy, d1.zw ) ;
    
                        vec2 uvs[4] = vec2[4]( d2.xy, d2.xw, d2.zw, d2.zy ) ;
                        var_uv.xy = uvs[gl_VertexID%4] ;
                        var_uv.z = d3.w ; // which texture layer
                        //var_uv.xy = sign( in_pos.xy ) *0.5 + 0.5 ;

                        var_col = d5 ;
                        vec4 pos = vec4( pos_p + frame * scaling * in_pos, 0.0, 1.0 )  ;
                        gl_Position = u_proj * u_view * u_world * pos ;
                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 300 es
                    precision mediump float ;
                    precision mediump sampler2DArray ;

                    in vec4 var_col ;
                    in vec3 var_uv ;
                    layout( location = 0 )out vec4 out_color ;

                    uniform sampler2DArray u_tex ;

                    void main()
                    {
                        out_color = texture( u_tex, var_uv ) * var_col ;
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
                        float4 col : COLOR0 ;
                        float3 uv : TEXCOORD0 ;
                    };
                            
                    Buffer< float4 > u_data ;

                    VS_OUTPUT VS( VS_INPUT input )
                    {
                        VS_OUTPUT output = (VS_OUTPUT)0 ;
                        int idx = (input.in_id / 4) * 6 + u_offset * 6 ;
                        float4 d0 = u_data.Load( idx + 0 ) ; // pos scale
                        float4 d1 = u_data.Load( idx + 1 ) ; // frame
                        float4 d2 = u_data.Load( idx + 2 ) ; // uv rect
                        float4 d3 = u_data.Load( idx + 3 ) ; // additional
                        float4 d4 = u_data.Load( idx + 4 ) ; // uv animation
                        float4 d5 = u_data.Load( idx + 5 ) ; // color
                        
                        float2 pivot = d3.xy * d0.zw * float2( 0.5, 0.5 ) ;
                        float2 scale = d0.zw * (d2.zw - d2.xy) ;
                        float2 pos_p = d0.xy + pivot ;

                        float2x2 frame = { d1.x, d1.y, 
                                           d1.z, d1.w } ;

                        float2 uvs[4] = { d2.xy, d2.xw, d2.zw, d2.zy } ;
                        output.uv.xy = uvs[input.in_id % 4] ;
                        output.uv.z = d3.w ;

                        output.col = d5 ;

                        float2 pos = mul( input.in_pos * scale, frame ) ; 
                        output.pos = float4( pos_p + pos, 0.0f, 1.0f )  ;
                        output.pos = mul( output.pos, u_world ) ;
                        output.pos = mul( output.pos, u_view ) ;
                        output.pos = mul( output.pos, u_proj ) ;
                        return output;
                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    
                    Texture2DArray u_tex : register( t0 ) ;
                    SamplerState smp_u_tex : register( s0 ) ;

                    struct VS_OUTPUT
                    {
                        float4 Pos : SV_POSITION ;
                        float4 col : COLOR0 ;
                        float3 uv : TEXCOORD0 ;
                    };

                    float4 PS( VS_OUTPUT input ) : SV_Target0
                    {
                        return u_tex.Sample( smp_u_tex, input.uv ) * input.col ;
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

void_t sprite_render_2d::set_texture( natus::ntd::string_cref_t name ) noexcept 
{
    _image_name = name ;
}

void_t sprite_render_2d::draw( size_t const l, natus::math::vec2f_cref_t pos, natus::math::mat2f_cref_t frame, natus::math::vec2f_cref_t scale, natus::math::vec4f_cref_t uv_rect, size_t const slot, natus::math::vec2f_cref_t pivot, natus::math::vec4f_cref_t color ) noexcept 
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
    d.pivot = pivot ;
    d.color = color ;

    _layers[l].sprites.emplace_back( std::move( d ) ) ;
    ++_num_sprites ;
}
            
void_t sprite_render_2d::prepare_for_rendering( void_t ) noexcept 
{
    bool_t vertex_realloc = false ;
    bool_t data_realloc = false ;
    bool_t reconfig_ro = false ;

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
    // one var set per render pass per layer
    {
        for( size_t i=_ro->get_num_variable_sets(); i<_render_data.size(); ++i )
        {
            this_t::add_variable_set( *_ro ) ;
            reconfig_ro = true ;
        }

        int_t offset = 0 ;
        for( size_t i=0; i<_render_data.size(); ++i )
        {
            _ro->get_variable_set(i)->data_variable<int32_t>( "u_offset" )->set( offset ) ;
            offset += int32_t( _render_data[i].num_quads ) ;
        }

        if( _image_name_changed || reconfig_ro )
        {
            for( size_t i=0; i<_render_data.size(); ++i )
            {
                _ro->get_variable_set(i)->texture_variable( "u_tex" )->set( _image_name ) ;
            }
            _image_name_changed = false ;
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

                // additional infos
                _ao->data_buffer().update< natus::math::vec4f_t >( idx + 3, 
                    natus::math::vec4f_t( sprites[i].pivot.x(), sprites[i].pivot.y(), 0.0f, float_t( sprites[i].slot ) ) ) ;

                // uv animation
                _ao->data_buffer().update< natus::math::vec4f_t >( idx + 4, 
                    natus::math::vec4f_t(1.0f) ) ;

                // color
                _ao->data_buffer().update< natus::math::vec4f_t >( idx + 5, 
                    sprites[i].color ) ;

                

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

            if( reconfig_ro ) a.configure( _ro ) ;
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
