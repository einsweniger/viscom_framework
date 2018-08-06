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
                           const std::function<bool(fs::path)> callback,
                           std::string basepath_suffix) :
            name{name},
            callback(callback)
    {
        for(auto pathstr : pathstrs) {
            auto base = fs::path{pathstr};
            if(0<basepath_suffix.length()) {
                base /= basepath_suffix;
            }
            if(!fs::is_directory(base)) {
              continue;
            }
            basepaths_.push_back(base);
            currentPath[base.string()] = base;
            pathContents[base.string()] = scan(base);
        }
    }

    void FileSelect::draw(bool *p_open) {

        if(!ImGui::Begin(name.c_str(), p_open)){ //}, ImGuiWindowFlags_MenuBar)) {
            ImGui::End();
            return;
        }

        auto childsize = ImVec2(ImGui::GetWindowContentRegionWidth() / basepaths_.size(), 0);
        filter.Draw();
        for (auto it = basepaths_.begin(); it != basepaths_.end(); ++it) {
            auto& base = (*it);
            ImGui::PushID(base.c_str());
            ImGui::BeginChild(base.string().c_str(),childsize, true, ImGuiWindowFlags_MenuBar);
            drawMenu(base);


            ImGui::TextUnformatted(currentPath[base.string()].string().c_str());
            for(auto& path : pathContents[base.string()]) {
                if (filter.PassFilter(path.string().c_str())){
                    if(ImGui::SmallButton(path.filename().string().c_str())) {
                        if(fs::is_directory(path)) {
                            currentPath[base.string()] = currentPath[base.string()] / path.filename() ;
                            pathContents[base.string()] = scan(path);
                            break;
                        } else if(callback(currentPath[base.string()] / path.filename())) {
                            *p_open = false;
                            break;
                        }
                    }
                }
            }

            ImGui::EndChild();
            ImGui::PopID();
            if(it != basepaths_.end()-1) {
                ImGui::SameLine(0.0f,0.0f);
            }
        }
        ImGui::End();

    }

    void FileSelect::drawMenu(fs::path &base) {
        if (ImGui::BeginMenuBar()) {
            if(ImGui::MenuItem("rescan")) {
                pathContents[base.string()] = scan(currentPath[base.string()]);
            }
            if(ImGui::MenuItem("up") && currentPath[base.string()].has_parent_path()) {
                if(currentPath[base.string()] != base) {
                    currentPath[base.string()] = currentPath[base.string()].parent_path();
                    pathContents[base.string()] = scan(currentPath[base.string()]);
                }
            }
            ImGui::EndMenuBar();
        }
    }
}
