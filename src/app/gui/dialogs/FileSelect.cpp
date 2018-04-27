//
// Created by bone on 25.04.18.
//

#include <imgui.h>
#include "FileSelect.h"
namespace minuseins::gui {

    std::vector<fs::path> FileSelect::scan(fs::path folder) {
        auto content = std::vector<fs::path>{};
        for(auto& p: fs::directory_iterator(folder)) {
            content.push_back(p);
        }
        return content;
    }

    FileSelect::FileSelect(const std::string &name, std::vector<std::string> pathstrs,
                           const std::function<bool(fs::path)> &callback,
                           std::string basepath_suffix) :
            name{name},
            callback(callback)
    {
        for(auto pathstr : pathstrs) {
            auto base = fs::path{pathstr + basepath_suffix};
            basepaths_.push_back(base);
            currentPath[base] = fs::path{"/"};
            pathContents[base] = scan(base);
        }
    }

    void FileSelect::draw(bool *p_open) {

        if(ImGui::Begin(name.c_str(), p_open, ImVec2(0,300),-1.0f, ImGuiWindowFlags_MenuBar)) {
            auto childsize = ImVec2(ImGui::GetWindowContentRegionWidth() / basepaths_.size(), 0);
            for (auto it = basepaths_.begin(); it != basepaths_.end(); ++it) {
                auto& base = (*it);
                ImGui::PushID(base.c_str());
                ImGui::BeginChild(base.c_str(),childsize, true, ImGuiWindowFlags_MenuBar);
                ImGui::BeginMenuBar();
                if(ImGui::MenuItem("rescan")) {
                    pathContents[base] = scan(base / currentPath[base]);
                }
                if(ImGui::MenuItem("up") && currentPath[base].has_parent_path()) {
                    currentPath[base] = currentPath[base].parent_path();
                    pathContents[base] = scan(base / currentPath[base]);
                }
                ImGui::EndMenuBar();
                ImGui::TextUnformatted(currentPath[base].c_str());
                for(auto& path : pathContents[base]) {
                    if(ImGui::SmallButton(path.filename().c_str())) {
                        if(fs::is_directory(path)) {
                            currentPath[base] = currentPath[base] / path.filename() ;
                            pathContents[base] = scan(base / currentPath[base]);
                            break;
                        } else if(callback(currentPath[base] / path.filename())) {
                            *p_open = false;
                            break;
                        }
                    }
                }

                ImGui::EndChild();
                ImGui::PopID();
                if(it != basepaths_.end()-1) {
                    ImGui::SameLine();
                }
            }
            ImGui::End();
        }
    }
}
