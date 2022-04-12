
#include "imgui.h"

namespace natus
{
    namespace tool
    {
        namespace imgui_custom
        {
        NATUS_TOOL_API bool ListBox(const char* label, int* current_item, int* hovered_item, const char* const items[], int items_count, int height_in_items = -1);

        NATUS_TOOL_API bool ListBox(const char* label, int* current_item, int* hovered_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items) ;

        NATUS_TOOL_API bool Selectable(const char* label, bool* hovered, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
        }
    }
}
