
#include "timeline.h"

#include <sstream>
#include <iomanip>

using namespace natus::tool ;

static auto tri_down = [&]( ImVec2 const pos, float_t const height, ImU32 const color, ImDrawList * draw_list )
{
    float_t const h = natus::math::fn<float_t>::ceil( height * 0.5f ) ;

    ImVec2 const points[] = 
    {
        pos + ImVec2(0.0f, 0.0f), pos + ImVec2( -h, -h ), pos + ImVec2( h, -h) 
    } ;

    draw_list->AddConvexPolyFilled( points, 3, color ) ;
} ;

static auto tri_up = [&]( ImVec2 const pos, float_t const height, ImU32 const color, ImDrawList * draw_list )
{
    float_t const h = natus::math::fn<float_t>::ceil( height * 0.5f ) ;

    ImVec2 const points[] = 
    {
        pos, pos + ImVec2( h, h ), pos + ImVec2( -h, h) 
    } ;

    draw_list->AddConvexPolyFilled( points, 3, color ) ;
} ;

timeline::timeline( void_t ) noexcept 
{
}

//***************************************************************
timeline::timeline( this_rref_t ) noexcept  
{
}

//***************************************************************
timeline::~timeline( void_t ) noexcept 
{
}

//***************************************************************
timeline::this_ref_t timeline::operator = ( this_cref_t ) noexcept 
{
    return *this ;
}

//***************************************************************
timeline::this_ref_t timeline::operator = ( this_rref_t ) noexcept 
{
    return *this ;
}

//***************************************************************
bool_t timeline::begin( natus::ntd::string_cref_t label, natus::tool::imgui_view_t ) noexcept 
{
    size_t const max_milli = 100000 ;
    _max_milli = max_milli ;
    float_t const top_line_height = 10.0f ;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImVec2 const scrolling_child_size = ImVec2( 0, ImGui::GetContentRegionAvail().y ) - ImVec2(0.0f, ImGui::GetTextLineHeight()+10) ;
    ImGui::BeginChild((label + "##timeline").c_str(), scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar) ;
    
    

    auto draw_list = ImGui::GetWindowDrawList() ;

    ImVec2 const capture_pos = ImGui::GetCursorScreenPos() ;
    float_t const height = ImGui::GetContentRegionAvail().y ;
    size_t const big_line_height = height * 0.5f ;
    size_t const small_line_height = height * 0.25f ;

    // set markers in order to have the horizontal scroll bar visible and scrollable
    {
        {
            ImVec2 pos = ImGui::GetCursorScreenPos() ;
            draw_list->AddLine( pos + ImVec2(0.0f,10.0f), pos + ImVec2( 0.0f,10.0f ), IM_COL32(255, 255, 255, 0), 1.0f ) ;
        }

        ImGui::SetCursorScreenPos( ImGui::GetCursorScreenPos() + ImVec2( this_t::milli_to_pixel(max_milli), 0.0f ) ) ;
                     
        {
            ImVec2 pos = ImGui::GetCursorScreenPos() ;
            draw_list->AddLine( pos + ImVec2(0.0f,0.0f), pos + ImVec2( 0.0f,10.0f ), IM_COL32(255, 255, 255, 255), 10.0f ) ;
        }

        ImGui::SetCursorScreenPos( capture_pos ) ;
    }
    
    float_t const scroll_max_x = ImGui::GetScrollMaxX() ;
    float_t const scroll_x = ImGui::GetScrollX() ;
    
    float_t const mouse_x = 
        std::max( 0.0f, std::min( ImGui::GetMousePos().x - ImGui::GetWindowPos().x + scroll_x, float_t( this_t::milli_to_pixel(max_milli) ) ) ) ;

    //natus::log::global_t::status( std::to_string(mouse_x) ) ;

    bool_t const mouse_in_cr = ImGui::IsMouseHoveringRect( 
        ImVec2( scroll_x, 0.0f) + ImGui::GetCursorScreenPos(), 
        ImVec2( scroll_x, 0.0f) + ImGui::GetCursorScreenPos()+ImGui::GetContentRegionAvail() ) ; 

    ImGui::SetCursorScreenPos( capture_pos ) ;

    // draw time line
    {
        float_t const small = 10.0f ;
        size_t const steps = 5 ;
        float_t const big = small * float_t(steps) ;

        float_t const offset_x = natus::math::fn<float_t>::fract( scroll_x / big ) * big ;
        ImVec2 const start = ImGui::GetCursorScreenPos() + ImVec2( scroll_x - offset_x, top_line_height ) ;

        // draw small lines
        {
            size_t const num_steps = size_t( ImGui::GetContentRegionAvail().x / small ) + steps ;
            for( size_t i=0; i<num_steps; ++i )
            {
                ImVec2 const p0 = start + ImVec2( small * float_t(i), 0.0f ) ;
                ImVec2 const p1 = start + ImVec2( small * float_t(i), small_line_height ) ;
                draw_list->AddLine( p0, p1, IM_COL32(150,150,150, 255), 1.0f ) ;
            }
        }

        ImGui::SetCursorScreenPos( capture_pos ) ;

        // draw big lines
        #if 1
        {
            size_t const the_big = size_t( scroll_x ) / size_t( big ) ;
            size_t const num_steps = size_t( ImGui::GetContentRegionAvail().x / big ) + 2 ;
            // need to draw one less (-1), otherwise, a infinite scroll appears
            for( size_t i=0; i<num_steps-1; ++i )
            {
                ImVec2 const p0 = start + ImVec2( big * float_t(i), 0.0f ) ;
                ImVec2 const p1 = start + ImVec2( big * float_t(i), big_line_height ) ;
                draw_list->AddLine( p0, p1, IM_COL32(200,200,200, 255), 1.0f ) ;
         
                size_t const cur_milli = this_t::pixel_to_milli( (the_big + i) * size_t(big) );

                #if 1
                size_t const sec = cur_milli / 1000 ;
                size_t const mil = cur_milli % 1000 ;

                ImGui::SetCursorScreenPos( p0 + ImVec2(2.0f, height * 0.25f) + ImVec2(0.0f, ((the_big+i)%2)*ImGui::GetTextLineHeight()*0.5) ) ;
                ImGui::Text( (std::to_string( sec ) + ":" + std::to_string( mil ) ).c_str() ) ;
                ImGui::SameLine() ;
                #endif
            }
        }
        #endif
    }

    ImGui::SetCursorScreenPos( capture_pos ) ;

    // line moving with mouse : hover
    if( mouse_in_cr )
    {
        ImGui::SetCursorScreenPos( ImGui::GetCursorScreenPos() + ImVec2( mouse_x, 0.0f ) ) ;

        ImVec2 const pos = ImGui::GetCursorScreenPos() ;

        {
            ImVec2 const p0 = pos ; 
            ImVec2 const p1 = p0 + ImVec2( 0.0f, top_line_height ) ;
            draw_list->AddLine( p0, p1, IM_COL32(255, 255, 0, 255), 1.0f ) ;

            _hover = this_t::pixel_to_milli( size_t( mouse_x ) ) ;
        }
        
    }

    ImGui::SetCursorScreenPos( capture_pos ) ;

    // current position marker
    {
        ImVec2 const pos = ImGui::GetCursorScreenPos() + ImVec2(0.0f, top_line_height) ;

        static size_t milli_test = 0 ;
        milli_test += 1 ;
        if( milli_test > max_milli ) milli_test = 0 ;

        if( ImGui::IsMouseDown(ImGuiMouseButton_Left) && mouse_in_cr )
        {
            milli_test = _hover ;
        }

        size_t const milli_pos = milli_test ;
        size_t const marker_pos = this_t::milli_to_pixel( milli_pos ) ;
        ImVec2 const avail = ImGui::GetContentRegionAvail() ;
        if( marker_pos > scroll_x && marker_pos < scroll_x + avail.x )
        {
            ImVec2 const p0 = pos + ImVec2( marker_pos, 0.0f ) ;
            ImVec2 const p1 = pos + ImVec2( marker_pos, big_line_height + ImGui::GetTextLineHeight() ) ;

            //tri_down( p0, top_line_height*0.5f, IM_COL32(0, 255, 0, 255), draw_list ) ;
            draw_list->AddLine( p0, p1, IM_COL32(0, 255, 0, 255), 1.0f ) ;
            tri_up( p1, top_line_height, IM_COL32(0, 255, 0, 255), draw_list ) ;
        }

        _play = milli_test ;
    }

    // move timeline with player marker
    if( _lock_player )
    {
        auto const p0 = this_t::milli_to_pixel( _play ) - ImGui::GetContentRegionAvail().x*0.5f ;
        auto const p = std::max( p0, 0.0f ) ;
        ImGui::SetScrollX( p ) ;
    }

    // testing line moving with scroll
    #if 0
    {
        ImGui::SetCursorScreenPos( ImGui::GetCursorScreenPos() + ImVec2( scroll_x, 0.0f ) ) ;

        {
            ImVec2 pos = ImGui::GetCursorScreenPos() ;
            draw_list->AddLine( pos + ImVec2(10.0f,0.0f), pos + ImVec2( 10.0f,10.0f ), IM_COL32(255, 0, 0, 255), 1.0f ) ;
        }
    }
    #endif

    ImGui::SetCursorScreenPos( capture_pos ) ;

    // testing a button within the timeline
    #if 0
    {
        ImVec2 const avail = ImGui::GetContentRegionAvail() ;
        ImGui::SetCursorScreenPos( ImGui::GetCursorScreenPos() + ImVec2( avail.x-100.0f, 0.0f ) ) ;

        ImGui::Button("test") ;

    }
    #endif 

    ImGui::SetCursorScreenPos( capture_pos ) ;

    

    //natus::log::global_t::status( "[" + std::to_string(scroll_max_x) + "] : [ " + std::to_string(scroll_x) + "] " ) ;

    
    return true ;
}

//***************************************************************
void_t timeline::end( void_t ) noexcept 
{
    ImGui::PopStyleVar() ;
    ImGui::EndChild();
    
    // play
    {
        natus::ntd::string_t const s = this_t::make_time_string( _play ) ;
        ImGui::TextColored( ImVec4(0.0f, 1.0f, 0.0f, 1.0f), s.c_str() ) ;
    }

    ImGui::SameLine() ;

    // hover
    {
        natus::ntd::string_t const s = this_t::make_time_string( _hover ) ;
        ImGui::TextColored( ImVec4(1.0f, 1.0f, 0.0f, 1.0f), s.c_str() ) ;
    }

    ImGui::SameLine() ;

    // max milli
    {
        natus::ntd::string_t const s = this_t::make_time_string( _max_milli ) ;
        ImGui::TextColored( ImVec4(1.0f, 0.0f, 0.0f, 1.0f), s.c_str() ) ;
    }
    
    ImGui::SameLine() ;

    {
        ImGui::Checkbox( "lock##timeline", &_lock_player ) ;
    }
}

//***************************************************************
size_t timeline::milli_to_pixel( size_t const milli ) const noexcept 
{
    return milli / _zoom ;
}

//***************************************************************
size_t timeline::pixel_to_milli( size_t const pixel ) const noexcept 
{
    return pixel * _zoom ;
}

//***************************************************************
natus::ntd::string_t timeline::make_time_string( size_t const milli ) const noexcept 
{
    natus::ntd::string_t s ;

    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << milli / 60000 ;
        s += ss.str();
    }
    s += ":" ;
    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << (milli / 1000) % 60 ;
        s += ss.str();
    }
    s += ":" ;
    {
        std::stringstream ss;
        ss << std::setw(3) << std::setfill('0') << milli % 1000 ;
        s += ss.str();
    }

    return s ;
}