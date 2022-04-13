
#include "imgui.h"

namespace natus
{
    namespace tool
    {
        namespace imgui_custom
        {
            NATUS_TOOL_API bool ListBox(const char* label, int* current_item, int* hovered_item, const char* const items[], int items_count, int height_in_items = -1);
        }
    }
}
