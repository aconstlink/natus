
#include "imgui.h"

namespace natus
{
    namespace tool
    {
        namespace imgui_custom
        {
            NATUS_TOOL_API bool ListBox(const char* label, int* current_item, int* hovered_item, size_t * double_clicked, 
                natus::ntd::vector<natus::ntd::string_t> & items, size_t & item_edited, int height_in_items = -1);
        }
    }
}
