
#include "player_controller.h"

using namespace natus::tool ;

//****************************************************************
void_t player_controller::begin( natus::tool::imgui_view_t ) noexcept 
{
    if( !_internal_play )
    {
        // play button
        if( ImGui::Button( "Play" ) )
        {
            _play = true ;
        }
    }
    else
    {
        // pause button 
        if( ImGui::Button( "Pause" ) )
        {
            _pause = true ;
        }
    }

    // stop button
    if( ImGui::Button( "Stop" ) )
    {
        _stop = true ;
    }
}

//****************************************************************
void_t player_controller::end( natus::tool::imgui_view_t ) noexcept 
{
    _play = false ;
    _pause = false ;
    _stop = false ;
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
bool_t player_controller::is_stop_clicked( void_t ) const noexcept 
{
    return _stop ;
}

//****************************************************************