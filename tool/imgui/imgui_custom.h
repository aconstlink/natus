
#include "imgui.h"

namespace natus
{
    namespace tool
    {
        namespace imgui_custom
        {
            NATUS_TOOL_API bool ListBox(const char* label, int* current_item, int* hovered_item, size_t & double_clicked, 
                natus::ntd::vector<natus::ntd::string_t> & items, size_t & item_edited, int height_in_items = -1);

            NATUS_TOOL_API bool ListBoxWithInputInt( const char* label, int* selected_item, int* hovered_item, size_t & double_clicked,
                natus::ntd::vector<std::pair< natus::ntd::string_t, int_t >> & items, size_t & item_edited, bool_t & value_changed, int height_items = -1 ) ;

            NATUS_TOOL_API bool Selectable(const char* label, bool* p_selected, bool_t * p_hovered, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
        }
    }
}
