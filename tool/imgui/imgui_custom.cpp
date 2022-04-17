
#include "imgui_custom.h"

#include "imgui/imgui_internal.h"

#include <natus/memory/guards/malloc_guard.hpp>

using namespace ImGui ;

bool natus::tool::imgui_custom::ListBox(const char* label, int* current_item, int* hovered_item, size_t * double_clicked, 
    natus::ntd::vector<natus::ntd::string_t> & items, size_t & item_edited, int height_items)
{
    bool_t selected = false ;

    if( ImGui::BeginListBox( label ) )
    {
        for( size_t i=0; i<items.size(); ++i )
        {
            natus::memory::malloc_guard<char_t> m( items[i].c_str(), items[i].size()+1 ) ;
            selected = i == *current_item ;
            
            if( *double_clicked == i )
            {
                natus::memory::malloc_guard<char_t> m2( 256 ) ;
                m2[0] = '\0' ;

                if( ImGui::InputText( m, m2, 256 ) )
                {}

                if( ImGui::IsKeyPressed( ImGuiKey_Enter ) )
                {
                    *double_clicked = size_t( -1 ) ;
                    items[i] = natus::ntd::string_t( m2, m2.size() ) ;
                    item_edited = i ;
                }
                else if( ImGui::IsKeyPressed( ImGuiKey_Escape ) )
                {
                    *double_clicked = size_t( -1 ) ;
                }
            }
            else if( ImGui::Selectable( m, &selected, ImGuiSelectableFlags_AllowDoubleClick ) )
            {
                if( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
                {
                    *double_clicked = i ;
                }
                else *current_item = i ;
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