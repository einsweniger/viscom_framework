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

    Tracker::Tracker(viscom::ApplicationNodeImplementation *appImpl) : appImpl(appImpl) {
        auto tmp = appImpl->bass->output_;
        decoder = std::make_unique<audio::BassDecoder>(appImpl->bass->openFile(viscom::Resource::FindResourceLocation("media/song.ogg", appImpl->GetApplication()),BASS_STREAM_DECODE | BASS_SAMPLE_MONO | BASS_POS_SCAN));
        appImpl->bass->output_ = tmp;
        std::cout << "text_height:" << ImGui::GetTextLineHeight() << std::endl;
    }


    template<class Tracks>
    void drawInvisibleTracks(Tracks& tracks) {
        for(auto& track : tracks) {
            if(track.second.visible) continue;
            ImGui::SameLine();
            if(ImGui::Selectable(track.first.c_str(),false,0,tracker::element_size())) {
                track.second.visible = true;
            }
        }
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
        drawInvisibleTracks(appImpl->tracks);
        drawInvisibleTracks(appImpl->namedTracks);
        drawInvisibleTracks(appImpl->vec3Tracks);

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

    template <class Tracks>
    void drawTrackHeader(Tracks& tracks) {
        for(auto& track : tracks) {
            if(!track.second.visible) continue;
            ImGui::SameLine();
            //TODO mute, fold?
            if(ImGui::Selectable(track.first.c_str(),false,0,tracker::element_size())) {
                track.second.visible=false;
            }
            if(ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(track.first.c_str());
                ImGui::EndTooltip();
            }

        }
    }
    void Tracker::draw_table() {
        // table header
        ImGui::Selectable("",false,0,ImVec2(128,ImGui::GetTextLineHeight()));
        ImGui::SameLine();
        ImGui::Text("row  ");
        drawTrackHeader(appImpl->tracks);
        drawTrackHeader(appImpl->namedTracks);
        drawTrackHeader(appImpl->vec3Tracks);

        // table content
        // draw row index
        ImGui::BeginChild("TrackerTable");
        ImGui::BeginGroup();
        for(auto& tex : decoder->textures) {
            auto id = tex->GetId();
            auto tex_dim = tex->GetDimensions();
            auto seconds_per_image = tex_dim.y / audio::samples_per_second;
            auto rows_per_second = audio::row_rate;
            auto tex_height = seconds_per_image*rows_per_second*ImGui::GetTextLineHeight();
            //100 samples per second, 22.93 rows per second. 1024 samples in image
            ImGui::Image((void*)id, ImVec2(FFT_TEX_W, static_cast<float>(tex_height)));
        }
        ImGui::EndGroup();
        ImGui::SameLine();
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
        draw_table_content();
        ImGui::EndChild(); //TrackerTable

    }

    template<class Tracks>
    void drawTrackContent(Tracks& tracks, bool track_active_row, tracker::Row currentRow) {
        for(auto& [name, track] : tracks) {
            if(!track.visible) continue;
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::PushID(name.c_str());
            ImGuiListClipper clip(row_count, tracker::element_size().y);
            if(track_active_row) {
                auto offset = currentRow * tracker::element_size().y;
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + offset, 0.5f);
            }

            track.drawTrack(clip);

            ImGui::PopID();
            ImGui::EndGroup();
        }
    }

    void Tracker::draw_table_content() {
        drawTrackContent(appImpl->tracks, track_active_row, appImpl->currentRow);
        drawTrackContent(appImpl->namedTracks, track_active_row, appImpl->currentRow);
        drawTrackContent(appImpl->vec3Tracks, track_active_row, appImpl->currentRow);
//        for(auto& [name, track] : appImpl->tracks) {
//            if(!track.visible) continue;
//            ImGui::SameLine();
//            ImGui::BeginGroup();
//            ImGui::PushID(name.c_str());
//            ImGuiListClipper clip(row_count, tracker::element_size().y);
//            if(track_active_row) {
//                auto offset = appImpl->currentRow * tracker::element_size().y;
//                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + offset, 0.5f);
//            }
//
//            track.drawTrack(clip);
//
//            ImGui::PopID();
//            ImGui::EndGroup();
//        }
//        for(auto& [name, track] : appImpl->namedTracks) {
//            if(!track.visible) continue;
//            ImGui::SameLine();
//            ImGui::BeginGroup();
//            ImGui::PushID(name.c_str());
//            ImGuiListClipper clip(row_count, tracker::element_size().y);
//            if(track_active_row) {
//                auto offset = appImpl->currentRow * tracker::element_size().y;
//                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + offset, 0.5f);
//            }
//
//            track.drawTrack(clip);
//
//            ImGui::PopID();
//            ImGui::EndGroup();
//        }
//        for(auto& [name, track] : appImpl->vec3Tracks) {
//            if(!track.visible) continue;
//            ImGui::SameLine();
//            ImGui::BeginGroup();
//            ImGui::PushID(name.c_str());
//            ImGuiListClipper clip(row_count, tracker::element_size().y);
//            if(track_active_row) {
//                auto offset = appImpl->currentRow * tracker::element_size().y;
//                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + offset, 0.5f);
//            }
//
//            track.drawTrack(clip);
//
//            ImGui::PopID();
//            ImGui::EndGroup();
//        }
    }

}