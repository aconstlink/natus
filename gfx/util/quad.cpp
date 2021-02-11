
#include "quad.h"


using namespace natus::gfx ;


quad::quad( natus::ntd::string_cref_t name ) noexcept 
{
    _name = name ;
}

quad::quad( this_rref_t rhv ) noexcept 
{
    _ro = std::move( rhv._ro ) ;
    _go = std::move( rhv._go ) ;
    _name = std::move( rhv._name ) ;
}

quad::~quad( void_t ) noexcept 
{
}

void_t quad::set_view_proj( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) noexcept 
{
    {
        auto * var = _vars->data_variable< natus::math::mat4f_t >( "u_world" ) ;
        var->set( view ) ;
    }
    {
        auto * var = _vars->data_variable< natus::math::mat4f_t >( "u_proj" ) ;
        var->set( proj ) ;
    }
}

void_t quad::set_scale( float_t const s ) noexcept 
{
    _world *= s ;

    {
        auto * var = _vars->data_variable< natus::math::mat4f_t >( "u_world" ) ;
        var->set( _world ) ;
    }
}

void_t quad::set_texture( natus::ntd::string_cref_t name ) noexcept 
{
    auto * var = _vars->texture_variable( "u_tex" ) ;
    var->set( name ) ;
}

void_t quad::init( natus::graphics::async_views_t asyncs ) noexcept 
{
    // root render states
    {
        natus::graphics::state_object_t so = natus::graphics::state_object_t(
            _name + ".render_states" ) ;


        {
            natus::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = false ;
            rss.depth_s.ss.do_depth_write = false ;

            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = false ;
            rss.polygon_s.ss.ff = natus::graphics::front_face::clock_wise ;
            so.add_render_state_set( rss ) ;
        }

        _rs = std::move( so ) ;
        
        asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( _rs ) ;
        } ) ;
    }

    // geometry
    {
        auto vb = natus::graphics::vertex_buffer_t()
            .add_layout_element( natus::graphics::vertex_attribute::position, natus::graphics::type::tfloat, natus::graphics::type_struct::vec3 )
            .resize( 4 ).update<vertex>( [=] ( vertex* array, size_t const ne )
        {
            array[ 0 ].pos = natus::math::vec3f_t( -0.5f, -0.5f, 0.0f ) ;
            array[ 1 ].pos = natus::math::vec3f_t( -0.5f, +0.5f, 0.0f ) ;
            array[ 2 ].pos = natus::math::vec3f_t( +0.5f, +0.5f, 0.0f ) ;
            array[ 3 ].pos = natus::math::vec3f_t( +0.5f, -0.5f, 0.0f ) ;
        } );

        auto ib = natus::graphics::index_buffer_t().
            set_layout_element( natus::graphics::type::tuint ).resize( 6 ).
            update<uint_t>( [] ( uint_t* array, size_t const ne )
        {
            array[ 0 ] = 0 ;
            array[ 1 ] = 1 ;
            array[ 2 ] = 2 ;

            array[ 3 ] = 0 ;
            array[ 4 ] = 2 ;
            array[ 5 ] = 3 ;
        } ) ;

        _go = natus::graphics::geometry_object_t( _name + ".geometry",
            natus::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ) ;

        asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( _go ) ;
        } ) ;
    }

    // blit framebuffer render object
    {
        natus::graphics::render_object_t rc = natus::graphics::render_object_t( _name + ".render_object" ) ;

        // shader configuration
        {
            natus::graphics::shader_object_t sc( _name + ".shader" ) ;

            // shaders : ogl 3.0
            {
                natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                    set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 140
                    in vec3 in_pos ;
                    out vec2 var_tx ;

                    void main()
                    {
                        var_tx = sign( in_pos.xy ) * vec2( 0.5 ) + vec2( 0.5 )  ;
                        gl_Position = vec4( in_pos, 1.0 ) ;
                    } )" ) ).

                    set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 140
                    in vec2 var_tx ;
                    out vec4 out_color ;
                    uniform sampler2D u_tex ;

                    void main()
                    {
                        out_color = texture( u_tex, var_tx ) ;
                    } )" ) ) ;

                sc.insert( natus::graphics::backend_type::gl3, std::move( ss ) ) ;
            }

            // shaders : es 3.0
            {
                natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                    set_vertex_shader( natus::graphics::shader_t( R"(
                    #version 300 es
                    in vec3 in_pos ;
                    out vec2 var_tx ;

                    void main()
                    {
                        var_tx = sign( in_pos.xy ) * vec2( 0.5 ) + vec2( 0.5 )  ;
                        gl_Position = vec4( in_pos, 1.0 ) ;
                    } )" ) ).

                    set_pixel_shader( natus::graphics::shader_t( R"(
                    #version 300 es
                    precision mediump float ;
                    in vec2 var_tx ;
                    out vec4 out_color ;
                    uniform sampler2D u_tex ;

                    void main()
                    {
                        out_color = texture( u_tex, var_tx ) ;
                    } )" ) ) ;

                sc.insert( natus::graphics::backend_type::es3, std::move( ss ) ) ;
            }

            // shaders : hlsl 11(5.0)
            {
                natus::graphics::shader_set_t ss = natus::graphics::shader_set_t().

                    set_vertex_shader( natus::graphics::shader_t( R"(

                    struct VS_OUTPUT
                    {
                        float4 pos : SV_POSITION ;
                        float2 tx : TEXCOORD0 ;
                    };

                    VS_OUTPUT VS( float4 in_pos : POSITION )
                    {
                        VS_OUTPUT output = (VS_OUTPUT)0;
                        output.pos = float4( in_pos.xyz, 1.0f ) ;
                        output.tx = sign( in_pos.xy ) * float2( 0.5, 0.5 ) + float2( 0.5, 0.5 ) ;
                        return output;
                    } )" ) ).

                    set_pixel_shader( natus::graphics::shader_t( R"(
                            
                    Texture2D u_tex : register( t0 );
                    SamplerState smp_u_tex : register( s0 );
                    float sys_flipv_u_tex ;

                    struct VS_OUTPUT
                    {
                        float4 pos : SV_POSITION ;
                        float2 tx : TEXCOORD0 ;
                    };

                    float4 PS( VS_OUTPUT input ) : SV_Target
                    {
                        float2 uv = input.tx ;
                        uv.y = lerp( uv.y, 1.0 - uv.y, sys_flipv_u_tex ) ;
                        return u_tex.Sample( smp_u_tex, uv );
                    } )" ) ) ;

                sc.insert( natus::graphics::backend_type::d3d11, std::move( ss ) ) ;
            }

            // configure more details
            {
                sc
                    .add_vertex_input_binding( natus::graphics::vertex_attribute::position, "in_pos" ) ;
            }

            asyncs.for_each( [&]( natus::graphics::async_view_t a )
            {
                a.configure( sc ) ;
            } ) ;
        }

        {
            rc.link_geometry( _name + ".geometry" ) ;
            rc.link_shader( _name + ".shader" ) ;
        }

        // add variable set 
        #if 0 // maybe there need to be defaults
        {
            natus::graphics::variable_set_res_t vars = natus::graphics::variable_set_t() ;
            {
                auto* var = vars->texture_variable( "u_tex_0" ) ;
                var->set( "the_scene.0" ) ;
                //var->set( "checker_board" ) ;
            }
            {
                auto* var = vars->texture_variable( "u_tex_1" ) ;
                var->set( "the_scene.1" ) ;
                //var->set( "checker_board" ) ;
            }

            rc.add_variable_set( std::move( vars ) ) ;
        }
        #endif

        _ro = std::move( rc ) ;

        _ro->remove_variable_sets().add_variable_set( _vars ) ;
        asyncs.for_each( [&]( natus::graphics::async_view_t a )
        {
            a.configure( _ro ) ;
        } ) ;
    }
}

void_t quad::release( natus::graphics::async_views_t ) noexcept 
{
}

void_t quad::render( natus::graphics::async_views_t asyncs ) noexcept 
{
    asyncs.for_each( [&]( natus::graphics::async_view_t a )
    {
        a.use( _rs ) ;
        a.render( _ro ) ;
        a.use( natus::graphics::state_object_res_t() ) ;
    } ) ;
}
