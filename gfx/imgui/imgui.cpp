#include "imgui.h"

#include "../camera/pinhole_camera.h"


using namespace natus::gfx ;

//***
imgui::imgui( void_t ) 
{
    _vars.emplace_back( natus::graphics::variable_set_t() ) ;
}

//***
imgui::imgui( this_rref_t rhv ) 
{
    natus_move_member_ptr( _ctx, rhv ) ;

    _rc = ::std::move( rhv._rc ) ;
    _sc = ::std::move( rhv._sc ) ;

    _gc = ::std::move( rhv._gc ) ;

    _vars = ::std::move( rhv._vars ) ;
}

//***
imgui::~imgui( void_t ) 
{
    if( natus::core::is_not_nullptr(_ctx) )
    {
        ImGui::DestroyContext(_ctx);
    }
}

//***
void_t imgui::execute( exec_funk_t funk )
{    
    funk( _ctx ) ;
}

//***
void_t imgui::init( natus::graphics::async_view_t async ) 
{    
    _ctx = ImGui::CreateContext() ;

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

            .resize( 100 ) ;

        auto ib = natus::graphics::index_buffer_t().
            set_layout_element( natus::graphics::type::tuint ).resize( 6*100 ) ;

        _gc = natus::graphics::geometry_object_t( "natus.gfx.imgui",
            natus::graphics::primitive_type::triangles,
            ::std::move( vb ), ::std::move( ib ) ) ;

        async.configure( _gc ) ;
    }

    {
        _render_states = natus::graphics::state_object_t("system.imgui") ;
    }

    // shader config
    {
        natus::graphics::shader_object_t sc( "natus.gfx.imgui" ) ;

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
                    uniform sampler2D u_tex ;

                    in vec2 var_uv ;
                    in vec4 var_color ;

                    void main()
                    {    
                        out_color = var_color * texture( u_tex, var_uv ) ;
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

                    uniform sampler2D u_tex ;

                    in vec2 var_uv ;
                    in vec4 var_color ;

                    void main()
                    {    
                        out_color = var_color * texture( u_tex, var_uv ) ;
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
                            Texture2D u_tex : register( t0 );
                            SamplerState smp_u_tex : register( s0 );

                            struct VS_OUTPUT
                            {
                                float4 pos : SV_POSITION ;
                                float2 tx : TEXCOORD0 ;
                                float4 color : COLOR0 ;
                            };

                            float4 PS( VS_OUTPUT input ) : SV_Target
                            {
                                return u_tex.Sample( smp_u_tex, input.tx ) * input.color ;
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

        async.configure( sc ) ;
    }

    // image configuration
    {
        ImGuiIO& io = ImGui::GetIO();

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height ) ;
        io.Fonts->TexID = (ImTextureID) 0 ;
        natus::graphics::image_t img = natus::graphics::image_t( natus::graphics::image_t::dims_t( width, height, 1 ) )
            .update( [&] ( natus::graphics::image_ptr_t, natus::graphics::image_t::dims_in_t /*dims*/, void_ptr_t data_in )
        {
            ::std::memcpy( data_in, ( void_cptr_t ) pixels, size_t( width * height * 4 ) ) ;
        } ) ;

        _ic = natus::graphics::image_object_t( "system.imgui.font", ::std::move( img ) )
            .set_wrap( natus::graphics::texture_wrap_mode::wrap_s, natus::graphics::texture_wrap_type::clamp )
            .set_wrap( natus::graphics::texture_wrap_mode::wrap_t, natus::graphics::texture_wrap_type::clamp )
            .set_filter( natus::graphics::texture_filter_mode::min_filter, natus::graphics::texture_filter_type::linear )
            .set_filter( natus::graphics::texture_filter_mode::mag_filter, natus::graphics::texture_filter_type::linear );

        async.configure( _ic ) ;
    }

    // render configuration
    {
        natus::graphics::render_object_t rc( "natus.gfx.imgui" ) ;

        rc.link_geometry( "natus.gfx.imgui" ) ;
        rc.link_shader( "natus.gfx.imgui" ) ;
        
        // the variable set with id == 0 is the default
        // imgui variable set for rendering default widgets
        {
            auto* var = _vars[ 0 ]->texture_variable( "u_tex" ) ;
            var->set( "system.imgui.font" ) ;
        }

        rc.add_variable_set( _vars[0] ) ;

        _rc = std::move( rc ) ;
        async.configure( _rc ) ;
     }

    {
        
        this_t::do_default_imgui_init() ;

        ImGui::StyleColorsDark() ;
        
        
    }
}

//***
void_t imgui::begin( void_t ) 
{
    ImGui::SetCurrentContext( _ctx ) ;
    ImGui::NewFrame() ;
}

//***
void_t imgui::render( natus::graphics::async_view_t async ) 
{
    natus::gfx::pinhole_camera_t camera ;
    camera.orthographic( float_t( _width ), float_t(_height), 1.0f, 1000.0f ) ;
    
    for( auto & vars : _vars )
    {
        auto* var = vars->data_variable< natus::math::mat4f_t >( "u_proj" ) ;
        var->set( camera.mat_proj() ) ;
    }
    
    ImGui::Render() ;
    ImDrawData* draw_data = ImGui::GetDrawData() ;

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int_t const fb_width = ( int_t ) ( draw_data->DisplaySize.x * draw_data->FramebufferScale.x );
    int_t const fb_height = ( int_t ) ( draw_data->DisplaySize.y * draw_data->FramebufferScale.y );
    if( fb_width <= 0 || fb_height <= 0 )
        return;

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    {
        size_t size = 0 ;
        for( int n = 0; n < draw_data->CmdListsCount; n++ )
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[ n ];
            size += cmd_list->CmdBuffer.Size ;
        }
        
        _gc->vertex_buffer().resize( draw_data->TotalVtxCount ) ;
        _gc->index_buffer().resize( draw_data->TotalIdxCount ) ;

        {
            _render_states->resize( size ) ;
            natus::graphics::render_state_sets_t rss ;

            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = false ;
            rss.depth_s.ss.do_depth_write = false ;
            
            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = false ;
            rss.polygon_s.ss.ff = natus::graphics::front_face::counter_clock_wise ;
            rss.polygon_s.ss.cm = natus::graphics::cull_mode::back ;
            rss.polygon_s.ss.fm = natus::graphics::fill_mode::fill ;
            
            rss.scissor_s.do_change = true ;
            rss.scissor_s.ss.do_activate = true ;
            
            rss.blend_s.do_change = true ;
            rss.blend_s.ss.do_activate = true ;
            rss.blend_s.ss.src_blend_factor = natus::graphics::blend_factor::src_alpha ;
            rss.blend_s.ss.dst_blend_factor = natus::graphics::blend_factor::one_minus_src_alpha ;

            for( size_t i=0; i<_render_states->size(); ++i )
            {
                _render_states->set_render_states_set( i, rss ) ;
            }

            async.configure( _render_states ) ;
        }
    }

    // update geometry
    {
        size_t vb_off = 0 ;
        size_t ib_off = 0 ;

        // Update geometry: Everything is packed into a single vb/ib combo
        for( int n = 0; n < draw_data->CmdListsCount; n++ )
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[ n ];

            _gc->vertex_buffer().update<this_t::vertex>( [&] ( vertex* array, size_t const /*ne*/ )
            {
                for( int_t i = 0; i < cmd_list->VtxBuffer.Size; ++i )
                {
                    ImDrawVert const& imvert = cmd_list->VtxBuffer.Data[ i ] ;
                    auto const c = ImGui::ColorConvertU32ToFloat4( imvert.col ) ;

                    array[ vb_off + i ].pos = natus::math::vec2f_t(
                        imvert.pos.x - _width * 0.5f, -imvert.pos.y + _height * 0.5f ) ;
                    array[ vb_off + i ].uv = natus::math::vec2f_t( imvert.uv.x, imvert.uv.y ) ;
                    array[ vb_off + i ].color = natus::math::vec4f_t( c.x, c.y, c.z, c.w ) ;
                }
            } );

            _gc->index_buffer().update<uint_t>( [&] ( uint_t* array, size_t const /*ne*/ )
            {
                for( int_t i = 0 ; i < cmd_list->IdxBuffer.Size; ++i )
                {
                    array[ ib_off + i ] = uint_t( vb_off ) + cmd_list->IdxBuffer.Data[ i ] ;
                }
            } ) ;

            vb_off += cmd_list->VtxBuffer.Size ;
            ib_off += cmd_list->IdxBuffer.Size ;
        }
        async.update( _gc ) ;
    }

    size_t rs_id = 0 ;
    size_t offset = 0 ;
    

    // Render command lists
    for( int n = 0; n < draw_data->CmdListsCount; n++ )
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[ n ];

        for( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++ )
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[ cmd_i ];

            // Project scissor/clipping rectangles into framebuffer space
            ImVec4 clip_rect;
            clip_rect.x = ( pcmd->ClipRect.x - clip_off.x ) * clip_scale.x;
            clip_rect.y = ( pcmd->ClipRect.y - clip_off.y ) * clip_scale.y;
            clip_rect.z = ( pcmd->ClipRect.z - clip_off.x ) * clip_scale.x;
            clip_rect.w = ( pcmd->ClipRect.w - clip_off.y ) * clip_scale.y;

            if( _texture_added )
            {
                async.configure( _rc ) ;
                _texture_added = false ;
            }

            if( clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f )
            {
                // change scissor
                {
                    _render_states->access_render_state( rs_id,
                        [&] ( natus::graphics::render_state_sets_ref_t sets )
                    {
                        sets.scissor_s.ss.rect = natus::math::vec4ui_t( ( uint_t ) clip_rect.x, ( uint_t ) ( fb_height - clip_rect.w ), ( uint_t ) ( clip_rect.z - clip_rect.x ), ( uint_t ) ( clip_rect.w - clip_rect.y ) ) ;
                    } ) ;
                }

                {
                    async.use( _render_states, rs_id, false ) ;
                }

                // do rendering
                {
                    natus::graphics::backend_t::render_detail_t rd ;
                    rd.num_elems = pcmd->ElemCount ;
                    rd.start = offset ;
                    rd.varset = size_t( pcmd->TextureId ) ;
                    async.render( _rc, rd ) ;
                }

                {
                    async.use( natus::graphics::state_object_t() ) ;
                }
            }

            offset += pcmd->ElemCount ;
            rs_id++ ;
        }
    }

    
}

//****
void_t imgui::do_default_imgui_init( void_t ) 
{
    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_natus";

    io.DisplaySize = ImVec2( float_t( _width ), float_t( _height ) ) ;

    using key_t = natus::device::layouts::ascii_keyboard_t::ascii_key ;

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ ImGuiKey_Tab ] = size_t( key_t::tab ) ;
    io.KeyMap[ ImGuiKey_LeftArrow ] = size_t( key_t::arrow_left );
    io.KeyMap[ ImGuiKey_RightArrow ] = size_t( key_t::arrow_right );
    io.KeyMap[ ImGuiKey_UpArrow ] = size_t( key_t::arrow_up );
    io.KeyMap[ ImGuiKey_DownArrow ] = size_t( key_t::arrow_down );
    io.KeyMap[ ImGuiKey_PageUp ] = size_t( key_t::one_up );
    io.KeyMap[ ImGuiKey_PageDown ] = size_t( key_t::one_down );
    //io.KeyMap[ ImGuiKey_Home ] = size_t( key_t::ho );
    io.KeyMap[ ImGuiKey_End ] = size_t( key_t::sk_end );
    io.KeyMap[ ImGuiKey_Insert ] = size_t( key_t::insert );
    io.KeyMap[ ImGuiKey_Delete ] = size_t( key_t::k_delete );
    io.KeyMap[ ImGuiKey_Backspace ] = size_t( key_t::back_space );
    io.KeyMap[ ImGuiKey_Enter ] = size_t( key_t::k_return );
    io.KeyMap[ ImGuiKey_Escape ] = size_t( key_t::escape );
    io.KeyMap[ ImGuiKey_Space ] = size_t( key_t::space );
    io.KeyMap[ ImGuiKey_A ] = size_t( key_t::a );
    io.KeyMap[ ImGuiKey_C ] = size_t( key_t::c );
    io.KeyMap[ ImGuiKey_V ] = size_t( key_t::v );
    io.KeyMap[ ImGuiKey_X ] = size_t( key_t::x );
    io.KeyMap[ ImGuiKey_Y ] = size_t( key_t::y );
    io.KeyMap[ ImGuiKey_Z ] = size_t( key_t::z );


    /*
    io.SetClipboardTextFn = ;
    io.GetClipboardTextFn = ;
    io.ClipboardUserData = ;
    */

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
    /*GLFWerrorfun prev_error_callback = glfwSetErrorCallback( NULL );
    g_MouseCursors[ ImGuiMouseCursor_Arrow ] = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_TextInput ] = glfwCreateStandardCursor( GLFW_IBEAM_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_ResizeNS ] = glfwCreateStandardCursor( GLFW_VRESIZE_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_ResizeEW ] = glfwCreateStandardCursor( GLFW_HRESIZE_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_Hand ] = glfwCreateStandardCursor( GLFW_HAND_CURSOR );
    #if GLFW_HAS_NEW_CURSORS
    g_MouseCursors[ ImGuiMouseCursor_ResizeAll ] = glfwCreateStandardCursor( GLFW_RESIZE_ALL_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_ResizeNESW ] = glfwCreateStandardCursor( GLFW_RESIZE_NESW_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_ResizeNWSE ] = glfwCreateStandardCursor( GLFW_RESIZE_NWSE_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_NotAllowed ] = glfwCreateStandardCursor( GLFW_NOT_ALLOWED_CURSOR );
    #else
    g_MouseCursors[ ImGuiMouseCursor_ResizeAll ] = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_ResizeNESW ] = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_ResizeNWSE ] = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
    g_MouseCursors[ ImGuiMouseCursor_NotAllowed ] = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
    #endif*/
}

//****
void_t imgui::update( window_data_cref_t data ) 
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2( ( float_t ) data.width, ( float_t ) data.height );
    _width = data.width ;
    _height = data.height ;
}

//****
void_t imgui::update( natus::device::three_device_res_t dev ) 
{
    ImGuiIO& io = ImGui::GetIO();

    // handle mouse
    if( dev.is_valid() )
    {
        natus::device::three_device_t::layout_t mouse( dev ) ;
        // 1. mouse position
        {
            if( io.WantSetMousePos )
            {
                // @todo set mouse cursor
            }
            else
            {
                // transforms natus coords to imgui coords.
                // get_local origin : bottom left upwards
                // imgui needs origin : top left downwards
                natus::math::vec2f_t pos = ( mouse.get_local() -
                    natus::math::vec2f_t( 0.0f, 1.0f ) ) * natus::math::vec2f_t( 1.0f, -1.0f ) ;

                pos = pos * natus::math::vec2f_t(
                    float_t( io.DisplaySize.x ),
                    float_t( io.DisplaySize.y ) ) ;

                io.MousePos = ImVec2( pos.x(), pos.y() ) ;
            }
        }

        // 2. buttons
        {
            using layout_t = natus::device::three_device_t::layout_t ;

            for( size_t i = 0; i < 3; ++i )
            {
                io.MouseDown[ i ] = false ;
            }

            if( mouse.is_pressing( layout_t::button::left ) )
            {
                io.MouseDown[ 0 ] = true ;
            }
            if( mouse.is_pressing( layout_t::button::right ) )
            {
                io.MouseDown[ 1 ] = true ;
            }
            if( mouse.is_pressing( layout_t::button::middle ) )
            {
                io.MouseDown[ 2 ] = true ;
            }
        }

        // 3. wheel
        {
            //using layout_t = natus::device::three_device_t::layout_t ;

            float_t const m = 1.0f ; // io.KeyCtrl ? 1.0f : 2.5f ;
            float_t s ;
            if( mouse.get_scroll( s ) )
            {
                s = s / m ;
                if( io.KeyShift ) io.MouseWheelH = s ;
                else io.MouseWheel = s ;
            }
        }
    }
}

//****
void_t imgui::update( natus::device::ascii_device_res_t dev ) 
{
    using ks_t = natus::device::components::key_state ;
    using layout_t = natus::device::layouts::ascii_keyboard_t ;
    using key_t = layout_t::ascii_key ;

    // handle keyboard
    if( !dev.is_valid() ) return ;

    ImGuiIO& io = ImGui::GetIO();

    natus::device::layouts::ascii_keyboard_t keyboard( dev ) ;

    for( size_t i = 0; i < size_t( key_t::num_keys ); ++i )
    {
        auto const ks = keyboard.get_state( key_t( i ) ) ;

        if( ks == ks_t::none ) continue ;

        char_t c ;
        if( layout_t::convert_key_to_ascii_char( key_t(i), c ) )
        {
            io.KeysDown[ i ] = ks == ks_t::released ? false : true ;
            if( !io.KeysDown[ i ] ) io.AddInputCharacter( c ) ;
        }
        else if( layout_t::convert_key_to_ascii_number( key_t(i), c ) )
        {
            io.KeysDown[ i ] = ks == ks_t::released ? false : true ;
            if( !io.KeysDown[ i ] ) io.AddInputCharacter( c ) ;
        }
        else
        {
            io.KeysDown[ i ] = ks == ks_t::pressed || ks == ks_t::pressing ;
        }
    }

    io.KeyCtrl = io.KeysDown[ size_t( key_t::ctrl_left ) ] ||
        io.KeysDown[ size_t( key_t::ctrl_right ) ];

    io.KeyShift = io.KeysDown[ size_t( key_t::shift_left ) ] ||
        io.KeysDown[ size_t( key_t::shift_right ) ];

    io.KeyAlt = io.KeysDown[ size_t( key_t::alt_left ) ] ||
        io.KeysDown[ size_t( key_t::alt_right ) ];

    io.KeySuper = io.KeysDown[ size_t( key_t::win_win_left ) ] ||
        io.KeysDown[ size_t( key_t::win_win_right ) ] ;
}

//****
ImTextureID imgui::texture( natus::ntd::string_in_t name ) noexcept 
{
    size_t i = 0 ;

    for( auto& vars : _vars )
    {
        auto* var = vars->texture_variable( "u_tex" ) ;
        if( var->get() == name ) break ;

        i++ ;
    }

    // @note
    // places a new variable_set to the render configuration because
    // there is no other data that needs to be changed per variable_set,
    // except the data variable that are changed anyway.
    if( i == _vars.size() )
    {
        _vars.emplace_back( natus::graphics::variable_set_t() ) ;
        _rc->add_variable_set( _vars[ i ] ) ;
        auto * var = _vars[ i ]->texture_variable( "u_tex" ) ;
        var->set( name ) ;

        _texture_added = true ;
    }

    return ImTextureID(i) ;
}
