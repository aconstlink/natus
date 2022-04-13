
#include "imgui_custom.h"

#include "imgui/imgui_internal.h"

using namespace ImGui ;

bool natus::tool::imgui_custom::ListBox(const char* label, int* current_item, int* hovered_item, const char* const items[], int items_count, int height_items)
{
    bool_t selected = false ;

    if( ImGui::BeginListBox( label ) )
    {
        for( size_t i=0; i<items_count; ++i )
        {
            if( ImGui::Selectable( items[i], &selected ) )
            {
                *current_item = i ;
            }

            if( ImGui::IsItemHovered() )
            {
                *hovered_item = i ;
            }
        }

        EndListBox() ;
    }
    return selected ;
}