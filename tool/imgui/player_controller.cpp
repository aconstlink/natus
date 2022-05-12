
#include "player_controller.h"

#include <imgui/imgui_internal.h>

using namespace natus::tool ;

ImVec2 const operator + ( ImVec2 const & lhv, ImVec2 const & rhv ) noexcept
{
    return ImVec2( lhv.x + rhv.x, lhv.y + rhv.y ) ;
}

ImVec2 const operator - ( ImVec2 const & lhv, ImVec2 const & rhv ) noexcept
{
    return ImVec2( lhv.x - rhv.x, lhv.y - rhv.y ) ;
}

//****************************************************************
void_t player_controller::do_tool( natus::ntd::string_cref_t label_, natus::tool::imgui_view_t imgui ) noexcept 
{
    auto circle_button = [&]( natus::ntd::string_cref_t l, ImVec2 const & pos, float_t const r ) -> bool_t
    {
        ImGuiContext& ctx = *ImGui::GetCurrentContext() ;
        const ImGuiStyle& style = ctx.Style;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false ;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        //ImVec2 const pos = window->DC.CursorPos;

        const ImGuiID id = window->GetID( l.c_str() ) ;
        
        ImVec2 const size( 2.0f * r, 2.0f * r ) ;

        const ImRect bb( pos, pos + size ) ;
        ImGui::ItemSize( size, style.FramePadding.y ) ;
        if( !ImGui::ItemAdd( bb, id ) )
            return false ;

        ImGuiButtonFlags flags = 0 ;
        if( ctx.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat )
            flags |= ImGuiButtonFlags_Repeat;

        bool hovered, held ;
        bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, flags );
        
        ImU32 const hover_color[] = 
        {
            IM_COL32(255, 255, 255, 255),
            IM_COL32(255, 0, 0, 255)
        } ;
        
        {
            ImVec2 const pos1 = pos + ImVec2( r, r ) ;
            draw_list->AddCircle( pos, 3.0f, IM_COL32(255, 255, 255, 255), 0, 1.0f ) ;
            draw_list->AddCircle( pos1, r, hover_color[ hovered ? 1 : 0 ], 0, 2.0f ) ;
            draw_list->AddCircleFilled( pos1, r, IM_COL32(255, 255, 255, 255) ) ;
        }

        return pressed ;
    } ;

    auto draw_play = [&]( ImVec2 const & pos, float_t const r )
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow() ;
        ImDrawList* draw_list = ImGui::GetWindowDrawList() ;

        //ImVec2 const pos = window->DC.CursorPos ;
        
        ImVec2 const p1 = pos + ImVec2( 0.6f * r, 0.5f * r ) ; // top
        ImVec2 const p2 = pos + ImVec2( 0.6f * r, 1.5f * r ) ; // bottom
        ImVec2 const p3 = pos + ImVec2( 1.5f * r, 1.0f * r ) ; // right

        draw_list->AddLine( p1, p2, IM_COL32(0, 0, 0, 255), 1.0f ) ;
        draw_list->AddLine( p1, p3, IM_COL32(0, 0, 0, 255), 1.0f ) ;
        draw_list->AddLine( p2, p3, IM_COL32(0, 0, 0, 255), 1.0f ) ;

        {
            ImVec2 const points[] = { p1, p2, p3 } ;
            draw_list->AddConvexPolyFilled( points, 3, IM_COL32(0, 0, 0, 255) ) ;
        }
    } ;
    
    auto draw_stop = [&]( ImVec2 const & pos, float_t const r )
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow() ;
        ImDrawList* draw_list = ImGui::GetWindowDrawList() ;

        //ImVec2 const pos = window->DC.CursorPos;//ImGui::GetCursorScreenPos() ;
        
        ImVec2 const p2 = pos + ImVec2( 0.5f * r, 0.5f * r ) ; // top
        ImVec2 const p1 = pos + ImVec2( 0.5f * r, 1.5f * r ) ; // bottom
        ImVec2 const p3 = pos + ImVec2( 1.5f * r, 0.5f * r ) ; // top
        ImVec2 const p4 = pos + ImVec2( 1.5f * r, 1.5f * r ) ; // bottom

        draw_list->AddLine( p1, p2, IM_COL32(255, 255, 255, 255), 1.0f ) ;

        {
            ImVec2 const points[] = { p1, p2, p3, p4 } ;
            draw_list->AddConvexPolyFilled( points, 4, IM_COL32(0, 0, 0, 255) ) ;
        }
    } ;

    if( !_internal_play )
    {
        ImVec2 const pos = ImGui::GetCursorScreenPos() ;
        float_t const r = ImGui::GetTextLineHeight() ;
        if( circle_button( "play##" + label_, pos, r ) )
        {
            natus::log::global_t::status( "play" ) ;
            _play = true ;
            _pause = false ;
        }
        draw_play( pos, r ) ;
    }
    else
    {
        ImVec2 const pos = ImGui::GetCursorScreenPos() ;
        float_t const r = ImGui::GetTextLineHeight() ;
        if( circle_button( "pause##" + label_, pos, r ) )
        {
            natus::log::global_t::status( "pause" ) ;
            _pause = true ;
            _play = false ;
        }
    }

    ImGui::SameLine() ;

    // stop button
    ImVec2 const pos = ImGui::GetCursorScreenPos() ;
    float_t const r = ImGui::GetTextLineHeight() ;
    if( circle_button( "stop##" + label_, pos, r ) )
    {
        natus::log::global_t::status( "stop" ) ;
        _play = false ;
        _pause = false ;
    }
    draw_stop( pos, r ) ;
}

//****************************************************************
bool_t player_controller::is_play_clicked( void_t ) const noexcept 
{
    return _play ;
}

//****************************************************************
bool_t player_controller::is_pause_clicked( void_t ) const noexcept 
{
    return _pause ;
}


//****************************************************************