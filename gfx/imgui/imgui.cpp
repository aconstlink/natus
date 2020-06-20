#include "imgui.h"

#include "../camera/pinhole_camera.h"


using namespace natus::gfx ;

//***
imgui::imgui( void_t ) 
{
    
}

//***
imgui::imgui( this_rref_t rhv ) 
{
    natus_move_member_ptr( _ctx, rhv ) ;

    _rc = ::std::move( rhv._rc ) ;
    _rc_id = ::std::move( rhv._rc_id ) ;
    _sc_id = ::std::move( rhv._sc_id ) ;
    _sc = ::std::move( rhv._sc ) ;

    _gc = ::std::move( rhv._gc ) ;
    _gc_id = ::std::move( rhv._gc_id ) ;

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
    ImGui::SetCurrentContext( _ctx ) ;
    funk( _ctx ) ;
}

//***
void_t imgui::init( natus::gpu::async_view_ref_t async ) 
{    
    // geometry
    {
        // @see struct vertex in the header
        auto vb = natus::gpu::vertex_buffer_t()
            .add_layout_element( natus::gpu::vertex_attribute::position, 
                natus::gpu::type::tfloat, natus::gpu::type_struct::vec2 )

            .add_layout_element( natus::gpu::vertex_attribute::texcoord0, 
                natus::gpu::type::tfloat, natus::gpu::type_struct::vec2 )

            .add_layout_element( natus::gpu::vertex_attribute::color0, 
                natus::gpu::type::tfloat, natus::gpu::type_struct::vec4 )

            .resize( 100 ) ;

        auto ib = natus::gpu::index_buffer_t().
            set_layout_element( natus::gpu::type::tuint ).resize( 6*100 ) ;

        _gc = natus::gpu::geometry_configuration_t( "natus.gfx.imgui",
            natus::gpu::primitive_type::triangles,
            ::std::move( vb ), ::std::move( ib ) ) ;

        async.configure( _gc_id, _gc ) ;
    }

    // shader config
    {
        natus::gpu::shader_configuration_t sc( "natus.gfx.imgui" ) ;

        // shaders : ogl 3.0
        {
            sc.insert( natus::gpu::backend_type::gl3, natus::gpu::shader_set_t().

                set_vertex_shader( natus::gpu::shader_t( R"(
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
                        gl_Position = u_proj * u_view * vec4( in_pos, 0.0, 1.0 ) ;
                    } )" ) ).

                set_pixel_shader( natus::gpu::shader_t( R"(
                    #version 140
                            
                    out vec4 out_color ;
                    //uniform sampler2D u_tex ;
                        
                    in vec2 var_uv ;
                    in vec4 var_color ;

                    void main()
                    {    
                        out_color = var_color * vec4( var_uv, 0.0, 1.0 );//* texture( u_tex, var_uv ) ;
                    } )" ) ) 
            ) ;
        }

        // shaders : es 3.0
        {
            sc.insert( natus::gpu::backend_type::es3, natus::gpu::shader_set_t().

                set_vertex_shader( natus::gpu::shader_t( R"(
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

                set_pixel_shader( natus::gpu::shader_t( R"(
                    #version 300 es
                    precision mediump float ;
                    layout( location = 0 ) out vec4 out_color ;
                            
                    //uniform sampler2D u_tex ;

                    in vec2 var_uv ;
                    in vec4 var_color ;

                    void main()
                    {    
                        gl_FragColor = var_color * vec4( var_uv, 0.0, 1.0 ); //* texture( u_tex, var_uv ) ;
                    } )" ) ) 
            ) ;
        }

        // configure more details
        {
            sc
                .add_vertex_input_binding( natus::gpu::vertex_attribute::position, "in_pos" )
                .add_vertex_input_binding( natus::gpu::vertex_attribute::texcoord0, "in_uv" )
                .add_vertex_input_binding( natus::gpu::vertex_attribute::color0, "in_color" )
                .add_input_binding( natus::gpu::binding_point::view_matrix, "u_view" )
                .add_input_binding( natus::gpu::binding_point::projection_matrix, "u_proj" ) ;
        }

        async.configure( _sc_id, sc ) ;
    }

    // render configuration
    {
        natus::gpu::render_configuration_t rc( "natus.gfx.imgui" ) ;

        rc.link_geometry( "natus.gfx.imgui" ) ;
        rc.link_shader( "natus.gfx.imgui" ) ;
    
        _vars = natus::gpu::variable_set_t() ;
        {
            //vars->texture_variable( "tex_name", "shader_var_name" ) ;
        }
        //async.connect( _rc, _vars ) ;

        rc.add_variable_set( _vars ) ;

        _rc = ::std::move( rc ) ;
        async.configure( _rc ) ;
     }

    {
        _ctx = ImGui::CreateContext() ;
        this_t::do_default_imgui_init() ;

        ImGui::StyleColorsDark() ;

        ImGui::NewFrame() ;
    }
}

//***
void_t imgui::render( natus::gpu::async_view_ref_t async ) 
{
    natus::gfx::pinhole_camera_t camera ;
    camera.orthographic( 100.0f, 100.0f, 1.0f, 1000.0f ) ;
    {
        auto* var = _vars->data_variable< natus::math::mat4f_t >( "u_view" ) ;
        var->set( camera.mat_view() ) ;
    }

    
    {
        auto* var = _vars->data_variable< natus::math::mat4f_t >( "u_proj" ) ;
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

    // Render command lists
    for( int n = 0; n < draw_data->CmdListsCount; n++ )
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        // update geometry
        {
            _gc->vertex_buffer()
                .resize( cmd_list->VtxBuffer.Size )
                .update<this_t::vertex>( [&] ( vertex* array, size_t const ne )
            {
                for( size_t i = 0; i < ne; ++i )
                {
                    ImDrawVert const& imvert = cmd_list->VtxBuffer.Data[ i ] ;
                    auto const c = ImGui::ColorConvertU32ToFloat4( imvert.col ) ;

                    array[ i ].pos = natus::math::vec2f_t( imvert.pos.x, imvert.pos.y ) ;
                    array[ i ].uv = natus::math::vec2f_t( imvert.uv.x, imvert.uv.y ) ;
                    array[ i ].color = natus::math::vec4f_t( c.x, c.y, c.z, c.w ) ;
                }
            } );

            _gc->index_buffer()
                .resize( cmd_list->IdxBuffer.Size )
                .update<uint_t>( [&] ( uint_t* array, size_t const ne )
            {
                for( size_t i = 0; i < ne; ++i )
                {
                    array[ i ] = cmd_list->IdxBuffer.Data[ i ] ;
                }
            } ) ;
            async.update( _gc_id, _gc ) ;
        }
        
        for( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++ )
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[ cmd_i ];

            // Project scissor/clipping rectangles into framebuffer space
            ImVec4 clip_rect;
            clip_rect.x = ( pcmd->ClipRect.x - clip_off.x ) * clip_scale.x;
            clip_rect.y = ( pcmd->ClipRect.y - clip_off.y ) * clip_scale.y;
            clip_rect.z = ( pcmd->ClipRect.z - clip_off.x ) * clip_scale.x;
            clip_rect.w = ( pcmd->ClipRect.w - clip_off.y ) * clip_scale.y;

            if( clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f )
            {
                /*
                // Apply scissor/clipping rectangle
                glScissor( ( int ) clip_rect.x, ( int ) ( fb_height - clip_rect.w ), ( int ) ( clip_rect.z - clip_rect.x ), ( int ) ( clip_rect.w - clip_rect.y ) );

                // Bind texture, Draw
                glBindTexture( GL_TEXTURE_2D, ( GLuint ) ( intptr_t ) pcmd->TextureId );
                #if IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
                if( g_GlVersion >= 320 )
                    glDrawElementsBaseVertex( GL_TRIANGLES, ( GLsizei ) pcmd->ElemCount, sizeof( ImDrawIdx ) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, ( void* ) ( intptr_t ) ( pcmd->IdxOffset * sizeof( ImDrawIdx ) ), ( GLint ) pcmd->VtxOffset );
                else
                    #endif
                    glDrawElements( GL_TRIANGLES, ( GLsizei ) pcmd->ElemCount, sizeof( ImDrawIdx ) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, ( void* ) ( intptr_t ) ( pcmd->IdxOffset * sizeof( ImDrawIdx ) ) );
                    */

                natus::gpu::backend_t::render_detail_t rd ;
                rd.num_elems = pcmd->ElemCount ;
                rd.start = pcmd->VtxOffset ;
                rd.varset = 0 ;
                async.render( _rc, rd ) ;
            }
        }
    }

    ImGui::NewFrame() ;
}

//****
void_t imgui::do_default_imgui_init( void_t ) 
{
    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_natus";

    io.DisplaySize = ImVec2( 100.0f, 100.0f );

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    /*io.KeyMap[ ImGuiKey_Tab ] = GLFW_KEY_TAB;
    io.KeyMap[ ImGuiKey_LeftArrow ] = GLFW_KEY_LEFT;
    io.KeyMap[ ImGuiKey_RightArrow ] = GLFW_KEY_RIGHT;
    io.KeyMap[ ImGuiKey_UpArrow ] = GLFW_KEY_UP;
    io.KeyMap[ ImGuiKey_DownArrow ] = GLFW_KEY_DOWN;
    io.KeyMap[ ImGuiKey_PageUp ] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ ImGuiKey_PageDown ] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ ImGuiKey_Home ] = GLFW_KEY_HOME;
    io.KeyMap[ ImGuiKey_End ] = GLFW_KEY_END;
    io.KeyMap[ ImGuiKey_Insert ] = GLFW_KEY_INSERT;
    io.KeyMap[ ImGuiKey_Delete ] = GLFW_KEY_DELETE;
    io.KeyMap[ ImGuiKey_Backspace ] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ ImGuiKey_Space ] = GLFW_KEY_SPACE;
    io.KeyMap[ ImGuiKey_Enter ] = GLFW_KEY_ENTER;
    io.KeyMap[ ImGuiKey_Escape ] = GLFW_KEY_ESCAPE;
    io.KeyMap[ ImGuiKey_KeyPadEnter ] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ ImGuiKey_A ] = GLFW_KEY_A;
    io.KeyMap[ ImGuiKey_C ] = GLFW_KEY_C;
    io.KeyMap[ ImGuiKey_V ] = GLFW_KEY_V;
    io.KeyMap[ ImGuiKey_X ] = GLFW_KEY_X;
    io.KeyMap[ ImGuiKey_Y ] = GLFW_KEY_Y;
    io.KeyMap[ ImGuiKey_Z ] = GLFW_KEY_Z;*/

    /*
    io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = g_Window;
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
void_t imgui::change( window_data_cref_t data ) 
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2( ( float_t ) data.width, ( float_t ) data.height );
}