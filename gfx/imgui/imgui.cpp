#include "imgui.h"


using namespace natus::gfx ;

//***
imgui::imgui( void_t ) 
{
    
}

//***
imgui::imgui( this_rref_t rhv ) 
{
    natus_move_member_ptr( _ctx, rhv ) ;
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
void_t imgui::init( void_t ) 
{    
    _ctx = ImGui::CreateContext() ;
    ImGui::StyleColorsDark() ;
}

//***
void_t imgui::render( void_t ) 
{
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

        // update vertex buffer
        
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
            }
        }
    }
}

