
#include "text_render_2d.h"

using namespace natus::gfx ;


text_render_2d::text_render_2d( natus::ntd::string_cref_t name, natus::graphics::async_views_t asyncs ) noexcept
{
    _asyncs = std::move( asyncs ) ;
    _name = "natus.gfx.text_render_2d." + name ;
}

text_render_2d::text_render_2d( this_rref_t rhv ) noexcept
{
    _rc = std::move( rhv._rc ) ;
    _sc = std::move( rhv._sc ) ;
    _gc = std::move( rhv._gc ) ;
    _vars = std::move( rhv._vars ) ;
    _asyncs = std::move( rhv._asyncs ) ;
    _ga = std::move( rhv._ga ) ;
}

text_render_2d::~text_render_2d( void_t ) noexcept
{
}

void_t text_render_2d::init( natus::font::glyph_atlas_res_t ga ) noexcept
{
    _ga = std::move( ga ) ;  

    // geometry
    {
        // @see struct vertex in the header
        auto vb = natus::graphics::vertex_buffer_t()
            .add_layout_element( natus::graphics::vertex_attribute::position, 
                natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 )

            .add_layout_element( natus::graphics::vertex_attribute::texcoord0, 
                natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 )

            .add_layout_element( natus::graphics::vertex_attribute::color0, 
                natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 )

            .resize( num_quads << 2 ).update<vertex>( [=] ( vertex* array, size_t const ne )
            {
                size_t const num_quads_ = ne >> 2 ;

                for( size_t v = 0 ; v <num_quads_ ; ++v )
                {
                    size_t const vid = v << 2 ;

                    array[ vid + 0 ].pos = natus::math::vec2f_t( -0.5f, -0.5f ) ;
                    array[ vid + 1 ].pos = natus::math::vec2f_t( -0.5f, +0.5f ) ;
                    array[ vid + 2 ].pos = natus::math::vec2f_t( +0.5f, +0.5f ) ;
                    array[ vid + 3 ].pos = natus::math::vec2f_t( +0.5f, -0.5f ) ;

                    array[ vid + 0 ].uv = natus::math::vec2f_t( -0.0f, -0.0f ) ;
                    array[ vid + 1 ].uv = natus::math::vec2f_t( -0.0f, +1.0f ) ;
                    array[ vid + 2 ].uv = natus::math::vec2f_t( +1.0f, +1.0f ) ;
                    array[ vid + 3 ].uv = natus::math::vec2f_t( +1.0f, -0.0f ) ;

                    array[ vid + 0 ].color = natus::math::vec4f_t( 1.0f ) ;
                    array[ vid + 1 ].color = natus::math::vec4f_t( 1.0f ) ;
                    array[ vid + 2 ].color = natus::math::vec4f_t( 1.0f ) ;
                    array[ vid + 3 ].color = natus::math::vec4f_t( 1.0f ) ;
                }
            } ); ;

        auto ib = natus::graphics::index_buffer_t().
            set_layout_element( natus::graphics::type::tuint ).resize( 6*num_quads ).
                update<uint_t>( [=] ( uint_t* array, size_t const ne )
                {
                    size_t const num_quads_ = ne / 6 ;

                    for( size_t i=0; i<num_quads_; ++i )
                    {
                        size_t const idx = i * 6 ;
                        size_t const vid = i << 2 ;

                        array[ idx + 0 ] = vid + 0 ;
                        array[ idx + 1 ] = vid + 1 ;
                        array[ idx + 2 ] = vid + 2 ;

                        array[ idx + 3 ] = vid + 0 ;
                        array[ idx + 4 ] = vid + 2 ;
                        array[ idx + 5 ] = vid + 3 ;
                    }
                } ) ; 

        _gc = natus::graphics::geometry_object_t( this_t::name(),
            natus::graphics::primitive_type::triangles,
            std::move( vb ), std::move( ib ) ) ;

        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( _gc ) ;
        } ) ;
    }

    {
        _render_states = natus::graphics::state_object_t(this_t::name()) ;
    }

    // shader config
    {
        natus::graphics::shader_object_t sc( this_t::name() ) ;

        // shaders : ogl 3.0
        {
            sc.insert( natus::graphics::backend_type::gl3, natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 140

                    in vec2 in_pos ;
                    in vec2 in_uv ;
                    in vec4 in_color ;

                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;

                    out vec2 var_uv ;
                    out vec4 var_color ;

                    void main()
                    {
                        var_uv = in_uv ;
                        var_color = in_color ;
                        gl_Position = u_proj * vec4( in_pos, 0.0, 1.0 ) ;
                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 140

                    out vec4 out_color ;
                    uniform sampler2D atlas ;

                    in vec2 var_uv ;
                    in vec4 var_color ;

                    void main()
                    {    
                        out_color = var_color * texture( atlas, var_uv ) ;
                    } )" ) ) 
            ) ;
        }

        // shaders : es 3.0
        {
            sc.insert( natus::graphics::backend_type::es3, natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 300 es
                    precision mediump float ;
                    layout( location = 0 ) in vec2 in_pos ;
                    layout( location = 1 ) in vec2 in_uv ;
                    layout( location = 2 ) in vec4 in_color ;

                    uniform mat4 u_proj ;
                    uniform mat4 u_view ;

                    out vec2 var_uv ;
                    out vec4 var_color ;

                    void main()
                    {
                        var_uv = in_uv ;
                        var_color = in_color ;
                        gl_Position = u_proj * u_view * vec4( in_pos, 0.0, 1.0 ) ;
                    } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 300 es
                    precision mediump float ;
                    layout( location = 0 ) out vec4 out_color ;

                    uniform sampler2D atlas ;

                    in vec2 var_uv ;
                    in vec4 var_color ;

                    void main()
                    {    
                        out_color = var_color * texture( atlas, var_uv ) ;
                    } )" ) ) 
            ) ;
        }

        // shaders : hlsl 11
        {
            sc.insert( natus::graphics::backend_type::d3d11, natus::graphics::shader_set_t().

                set_vertex_shader( natus::graphics::shader_t( R"(
                            cbuffer Camera : register( b0 ) 
                            {
                                matrix u_proj ;
                                matrix u_view ;
                            }

                            struct VS_OUTPUT
                            {
                                float4 pos : SV_POSITION ;
                                float2 tx : TEXCOORD0 ;
                                float4 color : COLOR0 ;
                            };

                            VS_OUTPUT VS( float2 in_pos : POSITION, float2 in_uv : TEXCOORD0, 
                                        float4 in_color : COLOR0 )
                            {
                                float4 pos = float4( in_pos.xy, 0.0f, 1.0f ) ;
                                VS_OUTPUT output = (VS_OUTPUT)0;
                                output.pos = mul( pos, u_view );
                                output.pos = mul( output.pos, u_proj );
                                output.tx = in_uv ;
                                output.color = in_color ;
                                return output;
                            } )" ) ).

                set_pixel_shader( natus::graphics::shader_t( R"(
                            Texture2D atlas : register( t0 );
                            SamplerState smp_atlas : register( s0 );

                            struct VS_OUTPUT
                            {
                                float4 pos : SV_POSITION ;
                                float2 tx : TEXCOORD0 ;
                                float4 color : COLOR0 ;
                            };

                            float4 PS( VS_OUTPUT input ) : SV_Target
                            {
                                return atlas.Sample( smp_atlas, input.tx ) * input.color ;
                            } )" 
                ) ) ) ;
        }

        // configure more details
        {
            sc
                .add_vertex_input_binding( natus::graphics::vertex_attribute::position, "in_pos" )
                .add_vertex_input_binding( natus::graphics::vertex_attribute::texcoord0, "in_uv" )
                .add_vertex_input_binding( natus::graphics::vertex_attribute::color0, "in_color" )
                .add_input_binding( natus::graphics::binding_point::view_matrix, "u_view" )
                .add_input_binding( natus::graphics::binding_point::projection_matrix, "u_proj" ) ;
        }

        _asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( sc ) ;
        } ) ;
    }

    {
        natus::font::glyph_atlas_ref_t ga = *_ga ;
        natus::graphics::image_t img = natus::graphics::image_t( 
        natus::graphics::image_t::dims_t( ga.get_width(), ga.get_height(),1 ) )
            .update( [&]( natus::graphics::image_ptr_t, natus::graphics::image_t::dims_in_t dims, void_ptr_t data_in )
        {
            typedef natus::math::vector4< uint8_t > rgba_t ;
            auto* data_ = reinterpret_cast< rgba_t * >( data_in ) ;

            for( size_t i=0; i<ga.get_num_images(); ++i )
            {
                // at the moment, only 4 images possible
                // @todo allow more glyph atlas images
                size_t const iid = i % 4 ;
                        
                auto * raw = ga.get_image(iid) ;
                for( size_t y=0; y<dims.y(); ++y )
                {
                    for( size_t x=0; x<dims.x() ; ++x )
                    {
                        *data_ = rgba_t(0) ;
                        size_t idx = y * dims.y() + x ;
                        data_[idx][(iid)%4] = raw->get_plane()[idx] ;
                    }
                }
            }
        } ) ;

        auto ic = natus::graphics::image_object_t( this_t::name() + ".atlas",
            std::move( img ) )
            .set_wrap( natus::graphics::texture_wrap_mode::wrap_s, natus::graphics::texture_wrap_type::repeat )
            .set_wrap( natus::graphics::texture_wrap_mode::wrap_t, natus::graphics::texture_wrap_type::repeat )
            .set_filter( natus::graphics::texture_filter_mode::min_filter, natus::graphics::texture_filter_type::nearest )
            .set_filter( natus::graphics::texture_filter_mode::mag_filter, natus::graphics::texture_filter_type::nearest );

        _asyncs.for_each( [&]( natus::graphics::async_view_t a ) { a.configure( ic ) ; } ) ;
    }

    // render configuration
    {
        natus::graphics::render_object_t rc( this_t::name() ) ;

        rc.link_geometry( this_t::name() ) ;
        rc.link_shader( this_t::name() ) ;
        
        _vars.clear() ;
        natus::graphics::variable_set_t vs ;
        
        {
            auto* var = vs.texture_variable( "atlas" ) ;
            var->set( this_t::name() + ".atlas" ) ;
        }

        {
            auto* var = vs.texture_variable( "glyph_infos" ) ;
            var->set( this_t::name() + ".glyph_infos" ) ;
        }

        _vars.emplace_back( std::move( vs ) ) ;

        for( size_t i =0; i<_vars.size() ; ++i )
        {
            rc.add_variable_set( _vars[i] ) ;
        }

        _rc = std::move( rc ) ;
        _asyncs.for_each( [&]( natus::graphics::async_view_t a ) { a.configure( _rc ) ; } ) ;
     }
}
        
void_t text_render_2d::set_view_projection( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) 
{
}

void_t text_render_2d::set_view_projection( size_t const, natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) 
{
}


natus::gfx::result text_render_2d::draw_text( size_t const group, size_t const font_id, size_t const point_size, natus::math::vec2f_cref_t pos, natus::math::vec4f_cref_t color, natus::ntd::string_cref_t ) 
{
    return natus::gfx::result::ok ;
}

natus::gfx::result text_render_2d::prepare_for_rendering( void_t ) 
{
    return natus::gfx::result::ok ;
}

bool_t text_render_2d::need_to_render( size_t const ) const 
{
    return true ;
}

natus::gfx::result text_render_2d::render( size_t const ) 
{
    return natus::gfx::result::ok ;
}

natus::gfx::result text_render_2d::release( void_t ) 
{
    return natus::gfx::result::ok ;
}


