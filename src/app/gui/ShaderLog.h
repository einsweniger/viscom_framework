//
// Created by bone on 25.04.18.
//

#ifndef VISCOMFRAMEWORK_SHADERLOG_H
#define VISCOMFRAMEWORK_SHADERLOG_H


#include <imgui.h>

namespace minuseins::gui {
    struct ShaderLog
    {
        bool visible = false;
        ImGuiTextBuffer     Buf;
        ImVector<int>       LineOffsets;        // Index to lines offset
        bool                ScrollToBottom;

        void    Clear()     { Buf.clear(); LineOffsets.clear(); }

        void    AddLog(const char* fmt, ...) IM_FMTARGS(2);

        void    Draw(const char* title, bool* p_open = nullptr);
    };
}


#endif //VISCOMFRAMEWORK_SHADERLOG_H
