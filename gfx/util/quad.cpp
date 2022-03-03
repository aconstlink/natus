
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
    _ro->for_each( [&]( size_t const, natus::graphics::variable_set_res_t const & vars )
    {
        vars->data_variable< natus::math::mat4f_t >( "u_view" )->set( view ) ;
        vars->data_variable< natus::math::mat4f_t >( "u_proj" )->set( proj ) ;
    } ) ;
}

void_t quad::set_position( natus::math::vec2f_cref_t pos ) noexcept 
{
    _ro->for_each( [&]( size_t const, natus::graphics::variable_set_res_t const & vars )
    {
        auto * var = vars->data_variable< natus::math::mat4f_t >( "u_world" ) ;
        natus::math::mat4f_t world = var->get() ;
        world.set_column( 3, natus::math::vec3f_t( pos, 0.0f ) ) ;
        var->set( world ) ;
    } ) ;
}

void_t quad::set_position( size_t const vs, natus::math::vec2f_cref_t pos ) noexcept 
{
    if( vs >= _ro->get_num_variable_sets() ) return ; // could add new one

    auto * var = _ro->get_variable_set( vs )->data_variable< natus::math::mat4f_t >( "u_world" ) ;

    natus::math::mat4f_t world = var->get() ;
    world.set_column( 3, natus::math::vec3f_t( pos, 0.0f ) ) ;
    var->set( world ) ;
}

void_t quad::set_scale( natus::math::vec2f_cref_t s ) noexcept 
{
    _ro->for_each( [&]( size_t const, natus::graphics::variable_set_res_t const & vars )
    {
        auto * var = vars->data_variable< natus::math::mat4f_t >( "u_world" ) ;
        natus::math::mat4f_t world = var->get() ;
        world[0] = s.x() ;
        world[5] = s.y() ;
        var->set( world ) ;
    } ) ;
}

bool_t quad::set_texture( natus::ntd::string_cref_t name ) noexcept 
{
    _ro->for_each( [&]( size_t const, natus::graphics::variable_set_res_t const & vars )
    {
        vars->texture_variable( "u_tex" )->set( name ) ;
    } ) ;
    return true ;
}

bool_t quad::set_texture( size_t const vs, natus::ntd::string_cref_t name ) noexcept 
{
    if( vs >= _ro->get_num_variable_sets() ) return false ; 
    _ro->get_variable_set( vs )->texture_variable( "u_tex" )->set( name ) ;
    return true ;
}

bool_t quad::set_texcoord( natus::math::vec4f_cref_t tc ) noexcept 
{
     _ro->for_each( [&]( size_t const, natus::graphics::variable_set_res_t const & vars )
    {
        vars->data_variable<natus::math::vec4f_t>( "u_tc" )->set( tc ) ;
    } ) ;
    return true ;
}

bool_t quad::set_texcoord( size_t const vs, natus::math::vec4f_cref_t tc ) noexcept 
{
    if( vs >= _ro->get_num_variable_sets() ) return false ;
    _ro->get_variable_set( vs )->data_variable<natus::math::vec4f_t>( "u_tc" )->set( tc ) ;
    return true ;
}

void_t quad::init( natus::graphics::async_views_t asyncs, size_t const nvs ) noexcept 
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
                    uniform mat4 u_world ;
                    uniform mat4 u_view ;
                    uniform mat4 u_proj ;
                    uniform vec4 u_tc ;
                    void main()
                    {
                        vec2 tc[4] = vec2[](
                            vec2( u_tc.x, u_tc.y ),
                            vec2( u_tc.x, u_tc.w ),
                            vec2( u_tc.z, u_tc.w ),
                            vec2( u_tc.z, u_tc.y )
                        ) ;
                        var_tx = tc[ gl_VertexID ] ;
                        gl_Position =  u_proj * u_view * u_world * vec4( sign( in_pos ), 1.0 ) ;
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
                    uniform mat4 u_world ;
                    uniform mat4 u_view ;
                    uniform mat4 u_proj ;
                    void main()
                    {
                        vec2 tc[4] = vec2[](
                            vec2( u_tc.x, u_tc.y ),
                            vec2( u_tc.x, u_tc.w ),
                            vec2( u_tc.z, u_tc.w ),
                            vec2( u_tc.z, u_tc.y )
                        ) ;
                        var_tx = tc[ gl_VertexID ] ;
                        gl_Position = u_world * u_view * u_proj * vec4( sign( in_pos ), 1.0 ) ;
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

                    cbuffer ConstantBuffer : register( b0 ) 
                    {
                        float4x4 u_proj ;
                        float4x4 u_view ;
                        float4x4 u_world ;
                        float4 u_tc ;
                    }

                    struct VS_OUTPUT
                    {
                        float4 pos : SV_POSITION ;
                        float2 tx : TEXCOORD0 ;
                    };

                    VS_OUTPUT VS( float4 in_pos : POSITION, uint in_id : SV_VertexID )
                    {
                        float2 tc[4] = {
                            float2( u_tc.x, u_tc.y ),
                            float2( u_tc.x, u_tc.w ),
                            float2( u_tc.z, u_tc.w ),
                            float2( u_tc.z, u_tc.y )
                        } ;

                        VS_OUTPUT output = (VS_OUTPUT)0;
                        output.pos = float4( sign( in_pos.xyz ), 1.0f ) ;
                        output.pos = mul( output.pos, u_world ) ;
                        output.pos = mul( output.pos, u_view ) ;
                        output.pos = mul( output.pos, u_proj ) ;
                        output.tx = tc[in_id] ; //sign( in_pos.xy ) * float2( 0.5, 0.5 ) + float2( 0.5, 0.5 ) ;
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

        _ro->remove_variable_sets() ;

        for( size_t i = 0 ; i<nvs; ++i )
        {
            natus::graphics::variable_set_res_t vars = natus::graphics::variable_set_t() ; 

            {
                auto * var = vars->data_variable< natus::math::mat4f_t >( "u_world" ) ;
                var->set( _world ) ;
            }
            {
                auto * var = vars->data_variable< natus::math::mat4f_t >( "u_view" ) ;
                var->set( _view ) ;
            }
            {
                auto * var = vars->data_variable< natus::math::mat4f_t >( "u_proj" ) ;
                var->set( _proj ) ;
            }
            {
                auto * var = vars->data_variable< natus::math::vec4f_t >( "u_tc" ) ;
                var->set( natus::math::vec4f_t(0.0f,0.0f,1.0f,1.0f) ) ;
            }
            rc.add_variable_set( vars ) ;
        }

        _ro = std::move( rc ) ;
        asyncs.for_each( [&]( natus::graphics::async_view_t a ) { a.configure( _ro ) ; } ) ;
    }
}

void_t quad::release( natus::graphics::async_views_t ) noexcept 
{
}

void_t quad::render( natus::graphics::async_views_t asyncs ) noexcept 
{
    asyncs.for_each( [&]( natus::graphics::async_view_t a )
    {
        a.push( _rs ) ;
        _ro->for_each( [&]( size_t const i, natus::graphics::variable_set_res_t const & )
        {
            natus::graphics::backend::render_detail rd ;
            rd.varset = i ;

            a.render( _ro, rd ) ;
        } ) ;
        
        a.pop( natus::graphics::backend::pop_type::render_state ) ;
    } ) ;
}

void_t quad::render( size_t const i, natus::graphics::async_views_t asyncs ) noexcept 
{
    asyncs.for_each( [&]( natus::graphics::async_view_t a )
    {
        a.push( _rs ) ;
        {
            natus::graphics::backend::render_detail rd ;
            rd.varset = i ;

            a.render( _ro, rd ) ;
        }
        a.pop( natus::graphics::backend::pop_type::render_state ) ;
    } ) ;
}

void_t quad::add_variable_sets( natus::graphics::async_views_t asyncs, size_t const nvs ) noexcept
{
    for( size_t i = 0 ; i<nvs; ++i )
    {
        natus::graphics::variable_set_res_t vars = natus::graphics::variable_set_t() ; 

        {
            auto * var = vars->data_variable< natus::math::mat4f_t >( "u_world" ) ;
            var->set( _world ) ;
        }
        {
            auto * var = vars->data_variable< natus::math::mat4f_t >( "u_view" ) ;
            var->set( _view ) ;
        }
        {
            auto * var = vars->data_variable< natus::math::mat4f_t >( "u_proj" ) ;
            var->set( _proj ) ;
        }
        {
            auto * var = vars->data_variable< natus::math::vec4f_t >( "u_tc" ) ;
            var->set( natus::math::vec4f_t(0.0f,0.0f,1.0f,1.0f) ) ;
        }
        _ro->add_variable_set( vars ) ;
    }

    asyncs.for_each( [&]( natus::graphics::async_view_t a ) { a.configure( _ro ) ; } ) ;
}

size_t quad::get_num_variable_sets( void_t ) const noexcept 
{
    return _ro->get_num_variable_sets() ;
}