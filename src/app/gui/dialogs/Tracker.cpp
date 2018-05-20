//
// Created by bone on 20.05.18.
//

#include <imgui.h>
#include <app/tracker/Track.h>
#include <app/ApplicationNodeImplementation.h>
#include <core/glfw.h>
#include "Tracker.h"

namespace minuseins::gui {
    constexpr unsigned int FFT_TEX_W = 128;

    constexpr size_t min(size_t a, size_t b) {
        if(a < b) {
            return a;
        }
        return b;
    }

    Tracker::Tracker(viscom::ApplicationNodeImplementation *appImpl) : appImpl(appImpl) {
        auto tmp = appImpl->bass->output_;
        decoder = std::make_unique<audio::BassDecoder>(appImpl->bass->openFile({"../resources/media/song.ogg"},BASS_STREAM_DECODE | BASS_SAMPLE_MONO | BASS_POS_SCAN));
        appImpl->bass->output_ = tmp;
        auto texDescr = viscom::enh::TextureDescriptor{sizeof(uint32_t), gl::GL_RGBA8, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE};
        tex = std::make_unique<viscom::enh::GLTexture>(FFT_TEX_W,minuseins::audio::FFT_SIZE, texDescr, &decoder->result[0]);
        std::cout << "text_height:" << ImGui::GetTextLineHeight() << std::endl;
    }

    bool Tracker::Draw(bool *p_open) {
        for(int i=0; i<128; i++) {
            decoder->step();
        }
        if(!*p_open) return false;
        if(!ImGui::Begin("Tracker", p_open)) {
            ImGui::End();
            return false;
        }

        ImGui::Checkbox("Track", &track_active_row);
        ImGui::SameLine();
        ImGui::DragFloat("tex_height", &texture_height);

        ImGui::PushItemWidth(100);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing,ImVec2(0,0));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,ImGuiCol_TitleBg);
        draw_table();

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(2);
        ImGui::PopItemWidth();
        ImGui::End();
        return true;
    }

    ImVec2 element_size() {
        return ImVec2(100, ImGui::GetTextLineHeight());
    }

    void Tracker::draw_table() {
        // table header
        ImGui::Selectable("",false,0,ImVec2(128,ImGui::GetTextLineHeight()));
        auto rowsize = ImGui::GetItemRectSize();
        ImGui::SameLine();
        ImGui::Text("row  ");
        for(auto& track : appImpl->tracks) {
            ImGui::SameLine();
            //TODO mute, fold?
            ImGui::Selectable(track.first.c_str(),false,0,element_size());
        }
        auto avail_width = ImGui::GetContentRegionAvailWidth();
        tex->SetData(&decoder->result[0]);
        auto id = tex->GetId();
        auto tex_dim = tex->GetDimensions();
        auto seconds_per_image = tex_dim.y / audio::samples_per_second;
        auto rows_per_second = audio::row_rate;
        auto tex_height = seconds_per_image*rows_per_second*ImGui::GetTextLineHeight();
        //100 samples per second, 22.93 rows per second. 1024 samples in image
        ImGui::Image((void*)id, ImVec2(FFT_TEX_W, static_cast<float>(tex_height)));
        ImGui::SameLine();
        // table content
        // draw row index
        ImGui::BeginChild("TrackerTable",ImVec2(avail_width-FFT_TEX_W,0));
        {
            const char* textfmt = "%04d ";
            ImGuiListClipper clip(row_count);
            ImGui::BeginGroup();
            while(clip.Step()) {
                for(auto line = clip.DisplayStart; line < clip.DisplayEnd; line++) {
                    if(line != appImpl->currentRow) {
                        ImGui::Text(textfmt, line);
                    } else {
                        ImGui::TextColored(ImVec4(1,0,1,1),textfmt, line);
                    }
                }
            }
            ImGui::EndGroup();
        }
        draw_table_content();
        ImGui::EndChild(); //TrackerTable

    }

    ImVec4 get_text_color(tracker::interpolation_type type) {
        switch (type) {
            case tracker::interpolation_type::Step:return ImVec4(1,1,1,1);
            case tracker::interpolation_type::Linear:return ImVec4(1.000f, 0.430f, 0.350f, 1.000f);
            case tracker::interpolation_type::Smooth:return ImVec4(0.000f, 1.000f, 0.000f, 1.000f);
            case tracker::interpolation_type::Ramp:return ImVec4(0.000f, 0.618f, 1.000f, 1.000f);
        }
        return ImVec4(1,1,1,1);
    }

    void Tracker::draw_table_content() {
        for(auto& [name, track] : appImpl->tracks) {
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::PushID(name.c_str());
            ImGui::PushStyleColor(ImGuiCol_Text, get_text_color(tracker::interpolation_type::Step));
            ImGuiListClipper clip(row_count, element_size().y);
            if(track_active_row) {
                auto offset = appImpl->currentRow * element_size().y;
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + offset, 0.5f);
            }

            while(clip.Step()) {
                //once here, there's at least one key.
                const size_t clip_end = static_cast<const size_t>(clip.DisplayEnd);
                size_t active_row = static_cast<const size_t>(clip.DisplayStart);
                size_t upper = clip_end;
                if(!track.keys.empty()) {
                    upper = min(track.keys.at(0).row, clip_end);
                }
                //draw everything before first key;
                while (active_row < upper) {
                    draw_empty_selectable(track,active_row, tracker::interpolation_type::Step);
                    active_row++;
                }

                for (size_t i = 0; i < track.keys.size(); ++i) {
                    auto& key = track.keys.at(i);
                    ImGui::PushStyleColor(ImGuiCol_Text, get_text_color(key.interp.type));
                    //draw the value.
                    if(active_row != clip_end && active_row == key.row) {
                        draw_value_selectable(track,key,active_row);
                        active_row++;
                    }

                    //draw until next key or end
                    if(i+1 < track.keys.size()) {
                        upper = min(track.keys.at(i+1).row, clip_end);
                    } else {
                        auto color = get_text_color(key.interp.type);
                        color.x /=2;
                        color.y /=2;
                        color.z /=2;
                        ImGui::PopStyleColor(1);
                        ImGui::PushStyleColor(ImGuiCol_Text, color);
                        upper = clip_end;
                    }

                    while(active_row < upper) {
                        draw_empty_selectable(track, active_row, key.interp.type);
                        if(ImGui::IsItemHovered()) {
                            if (ImGui::IsKeyPressed(GLFW_KEY_I, false)) {
                                key.nextInterpolation();
                            }
                        }
                        active_row++;
                    }
                    ImGui::PopStyleColor(1);
                }
            }
            ImGui::PopStyleColor(1);
            ImGui::PopID();
            ImGui::EndGroup();
        }
    }

    void Tracker::draw_value_selectable(tracker::Track& track, tracker::Key& key, size_t active_row) {
        bool focus = false;
        ImGui::PushID(static_cast<int>(active_row));
        if(ImGui::Selectable(std::to_string(key.value).c_str(),false,0,element_size())) {
            focus = true;
            ImGui::OpenPopup("value_edit");
        }
        if(ImGui::IsItemHovered()) {
            if(ImGui::IsKeyPressed(GLFW_KEY_I, false)) {
                key.nextInterpolation();
            }
            if(ImGui::IsKeyPressed(GLFW_KEY_DELETE, false)) {
                track.delete_key(key.row);
            }
        }
        if(ImGui::BeginPopup("value_edit")) {
            if(focus) ImGui::SetKeyboardFocusHere();
            ImGui::InputFloat("value", &key.value);
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }

    void Tracker::draw_empty_selectable(tracker::Track track, size_t row, tracker::interpolation_type type) {
        ImGui::PushID(static_cast<int>(row));
        if(ImGui::Selectable("---", appImpl->currentRow == row, 0, element_size())) {
            auto newKey = tracker::Key{};
            newKey.row = static_cast<tracker::Row>(row);
            newKey.value = track.get_value(row);
            newKey.interp.type = type;
            track.set_key(std::move(newKey));
        }
        ImGui::PopID();
    }
}