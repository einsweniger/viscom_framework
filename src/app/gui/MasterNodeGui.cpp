//
//
// Created by bone on 15.04.18.
//

#include <core/resources/Resource.h>
#include <core/gfx/FrameBuffer.h>
#include <core/resources/ResourceManager.h>
#include <experimental/filesystem>
#include <app/shadertoy/ShaderToyLoader.h>
#include <glbinding/gl/types.h>
#include <glbinding/gl/gl.h>
#include <enh/ApplicationNodeBase.h>
#include <core/glfw.h>
#include <glbinding-aux/Meta.h>
#include <app/util.h>
#include <app/gfx/gl/ProgramInspector.h>
#include <app/MasterNode.h>
#include <app/gfx/IntrospectableFsq.h>
#include <app/gfx/gl/handlers.h>
#include <glbinding/glbinding.h>
#include <app/gui/dialogs/Animation.h>
#include "MasterNodeGui.h"
#include "app/gui/dialogs/FileSelect.h"
#include "app/gui/dialogs/Overlay.h"


namespace minuseins::gui {

    static Overlay overlay;

    MasterNodeGui::MasterNodeGui(viscom::ApplicationNodeImplementation *appImpl,
                                 viscom::ApplicationNodeInternal *appNode) :
            appImpl{appImpl},
            appNode{appNode}
    {
        try {
            auto config_file = viscom::Resource::FindResourceLocation(config_name,appNode);
        } catch (viscom::resource_loading_error&) {
            activeWindows["MainMenu"] = true;
        }
        //TODO use vector to draw menu and toggles.
        auto searchPaths = appNode->GetConfig().resourceSearchPaths_;
    }

    void MasterNodeGui::init() {
        {  //set error callbacks
            using namespace glbinding;
            setCallbackMaskExcept(CallbackMask::After | CallbackMask::ParametersAndReturnValue, { "glGetError" });
            setAfterCallback(std::bind(&OglLog::callback, &log, std::placeholders::_1));
        }
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    }

    void MasterNodeGui::UpdateFrame(double currentTime, double elapsedTime) {
        overlay.UpdateFrame(currentTime, elapsedTime);
    }

    void MasterNodeGui::Draw2D(viscom::FrameBuffer &fbo) {
        fbo.DrawToFBO([this]() {
            overlay.drawOverlay(&activeWindows["Overlay"]);
            drawMainMenu(&activeWindows["MainMenu"]);
            if(activeWindows["ImGuiDemo"]) {
                ImGui::ShowDemoWindow(&activeWindows["ImGuiDemo"]);
            }
            drawShaderToySelectImport(&activeWindows["ShaderToyImport"]);
            drawShaderWindow(&activeWindows["Shaders"]);
            drawTextureWindow(&activeWindows["Textures"]);
            drawTextureImportWindow(&activeWindows["TextureImport"]);
            drawShaderImport(&activeWindows["ShaderImport"]);
            drawGPUProgram(&activeWindows["GPUProgram"]);
            drawTimeSlider(&activeWindows["TimeSlider"]);
            drawAnimation(&activeWindows["AnimationManager"]);
            drawGlobalVars(&activeWindows["Globals"]);
            drawTracker(&activeWindows["Tracker"]);
//            if(activeWindows["AnimationManager"]) {
//                animManager.ShowAnimationMenu("AnimationManager", activeWindows["AnimationManager"]);
//            }

            log.Draw("OGLerrors", &activeWindows["OGLerrors"]);
            log.Clear();
        });
    }

    void MasterNodeGui::drawMainMenu(bool *p_open) {
        if(*p_open && ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::BeginMenu("import")) {
                    ImGui::MenuItem("ShaderToy","", &activeWindows["ShaderToyImport"]);
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("open")) {
                    ImGui::MenuItem("shader","", &activeWindows["ShaderImport"]);
                    ImGui::MenuItem("texture","", &activeWindows["TextureImport"]);
                    ImGui::End();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Scene")) {
                //ImGui::MenuItem("NewScene", "", &activeWindows["NewScene"]);
                ImGui::MenuItem("Tracker","", &activeWindows["Tracker"]);
                ImGui::MenuItem("Global Vars","", &activeWindows["Globals"]);
                ImGui::MenuItem("Animations","", &activeWindows["AnimationManager"]);
                ImGui::MenuItem("StopTime", "SPACE", &appImpl->stopTime_);
                ImGui::MenuItem("DrawToy", "", &appImpl->drawToy);
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Shader")) {
                //ImGui::MenuItem("import");
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows")) {
                ImGui::MenuItem("Textures", "T", &activeWindows["Textures"]);
                ImGui::MenuItem("MainMenu", "M", &activeWindows["MainMenu"]);
                ImGui::MenuItem("Overlay", "O", &activeWindows["Overlay"]);
                ImGui::MenuItem("Shaders", "Ctrl+S", &activeWindows["Shaders"]);
                ImGui::MenuItem("GPUProgram","G", &activeWindows["GPUProgram"]);
                ImGui::MenuItem("TimeSlider", "H",&activeWindows["TimeSlider"]);
                ImGui::MenuItem("OGLerrors", "",&activeWindows["OGLerrors"]);
                ImGui::Separator();
                ImGui::MenuItem("ImGuiDemo", "", &activeWindows["ImGuiDemo"]);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void MasterNodeGui::toggle(std::string item) {
        activeWindows[item] = !activeWindows[item];
    }

    void MasterNodeGui::drawShaderToySelectImport(bool *p_open) {
        if(!*p_open) return;
        static FileSelect select{"Import ShaderToy",appNode->GetConfig().resourceSearchPaths_, [this](fs::path path) {
            return ShaderToyCallback(path);
        }};
        select.draw(p_open);
    }
    void MasterNodeGui::drawShaderWindow(bool *p_open) {
        if(!*p_open) return;
        if(!ImGui::Begin("Shaders", p_open)){
            ImGui::End();
            return;
        }

        ImGui::End();
    }

    void MasterNodeGui::drawShaderImport(bool *p_open) {
        if(!*p_open) return;
        static auto staged_shaders = std::vector<std::string>{};
        static std::string progname{};
        progname.reserve(50);
        //static auto& gm = appNode->GetGPUProgramManager();
        static FileSelect shader_select("Open Shader",appNode->GetConfig().resourceSearchPaths_, [&](fs::path path) {
            try {
                auto fsq = std::make_unique<IntrospectableFsq>(path.string());
                fsq->init(appImpl);
                appImpl->fsqs.push_back(std::move(fsq));
                return false;
            } catch (viscom::shader_compiler_error &err) {
                std::cout << err.what() << std::endl;
                return false;
            }
        }, "shader/");
        shader_select.draw(p_open);
    }

    void MasterNodeGui::drawTextureImportWindow(bool *p_open) {
        if(!*p_open) return;
        static FileSelect texture_select{"Open Texture",appNode->GetConfig().resourceSearchPaths_, [&](fs::path path) {
            return textureCallback(path);
        }};
        texture_select.draw(p_open);
    }

    void MasterNodeGui::drawTextureWindow(bool *p_open) {
        if(!*p_open) return;
        static viscom::ResourceManager<viscom::Texture>& tm = appNode->GetTextureManager();
        if(!ImGui::Begin("Textures", p_open)) {
            ImGui::End();
            return;
        }

        std::for_each(tm.cbegin(), tm.cend(), [](const auto& tex) {
            if(!tex.second.expired()) {
                std::shared_ptr<viscom::Texture> texture = tex.second.lock();
                ImGui::Text("texid: %d", texture->getTextureId());
                ImGui::SameLine();
                ImGui::TextUnformatted(tex.first.c_str());
                ImVec2 uv0(0, 1);
                ImVec2 uv1(1, 0);
                auto maxwidth = ImGui::GetContentRegionAvailWidth();
                auto texdim = texture->getDimensions();
                float aspect = texdim.x / texdim.y;
                if(aspect > 1.0) {aspect = 1.0f/aspect;};
                ImVec2 region(maxwidth, maxwidth * aspect);
                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) texture->getTextureId()), region, uv0, uv1);
            }
        });

        ImGui::End();
    }

    bool MasterNodeGui::textureCallback(fs::path path) {
        static auto& tm = appNode->GetTextureManager();
        try {
            openTextures.push_back(tm.GetResource(path.string()));
            return !appNode->IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
        } catch (viscom::resource_loading_error &err) {
            std::cout << err.errorDescription_ << std::endl;
            return false;
        }
    }


    bool MasterNodeGui::ShaderToyCallback(fs::path path) {
        //TODO clean loading!
        try {
            if (path.has_extension()
                && ".json" == path.extension().generic_string()) {
                auto realpath = viscom::Resource::FindResourceLocation(path.string(), appNode);
                loader = std::make_unique<shadertoy::ShaderToyLoader>(realpath);
                //appImpl->fsqs.clear();
                //TODO Renderpass.name is sometimes empty
                for(auto& buf : loader->buffers) {
                    std::cout << buf.name << std::endl;
                    auto outfile = fs::path{"shadertoy/"+loader->toy_->info.id +"/"+ buf.name + ".frag"};
                    auto tq = std::make_unique<minuseins::ShaderToyFsq>(outfile.string());
                    tq->loadParams(buf);
                    tq->init(appImpl);

                    appImpl->toys.push_back(std::move(tq));
                }
                auto outfile = fs::path{"shadertoy/"+loader->toy_->info.id +"/"+ loader->image.name + ".frag"};
                auto tq = std::make_unique<minuseins::ShaderToyFsq>(outfile.string());
                tq->loadParams(loader->image);
                tq->init(appImpl);

                appImpl->toys.push_back(std::move(tq));
                return false;
            } else { return false; }

        } catch (viscom::resource_loading_error& err) {
            std::cerr << err.errorDescription_ << std::endl;
            return false;
        } catch (viscom::shader_compiler_error& err) {
            std::cerr << err.what() << std::endl;
            return false;
        }

    }

    void MasterNodeGui::drawGPUProgram(bool *p_open) {
        if(!*p_open) return;
        {
            auto& fsqs = appImpl->fsqs;
            for (auto it = fsqs.begin(); it != fsqs.end();) {
                ImGui::PushID((*it)->fragmentShader.c_str());
                ImGui::Begin("GPUProgram");
                std::string buttonText = "x##"+(*it)->fragmentShader;
                if(ImGui::SmallButton(buttonText.c_str())) {
                    it = fsqs.erase(it);
                } else {
                    ImGui::SameLine();
                    (*it)->Draw2D(p_open);
                    ++it;
                }

                ImGui::End();
                ImGui::PopID();
            }
        }

        auto& toys = appImpl->toys;
        for (auto it = toys.begin(); it != toys.end();) {
            ImGui::PushID((*it)->fragmentShader.c_str());
            ImGui::Begin("ShaderToys");
            std::string buttonText = "x##"+(*it)->fragmentShader;
            if(ImGui::SmallButton(buttonText.c_str())) {
                it = toys.erase(it);
            } else {
                ImGui::SameLine();
                (*it)->Draw2D(p_open);
                ++it;
            }

            ImGui::End();
            ImGui::PopID();
        }

    }

    void MasterNodeGui::drawTimeSlider(bool *p_open) {
        if(!*p_open) return;
        auto height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetIO().DisplaySize.y - height));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, height));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

        auto flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings;
        if(ImGui::Begin("##TimeSlider", p_open, flags)) {
            ImGui::PushItemWidth(-1);
            //ImGui::DragFloat("##Time", &appImpl->currentTime_,0.0f, 7000.f);
            ImGui::DragInt("##Time", reinterpret_cast<int *>(&appImpl->currentRow), 1, 0,7000);
            if(ImGui::IsItemActive()) {
                appImpl->bass->set_row(appImpl->currentRow);
            }
            ImGui::PopItemWidth();
        }

        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    void MasterNodeGui::drawAnimation(bool *p_open) {
        if(!*p_open) return;
        static gui::Animation anim(appNode->GetConfig().resourceSearchPaths_.at(0), appNode->GetCamera());
        anim.draw(p_open);
    }

    void MasterNodeGui::drawGlobalVars(bool *p_open) {
        static float histogram_scale = 0.07;
        if(!*p_open) return;
        if(!ImGui::Begin("Global Variables")) {
            ImGui::End();
            return;
        }
        ImGui::DragFloat("fftSmoothing",&appImpl->fftSmootingFactor, 0.01f,0.0f,0.98f);
        ImGui::DragFloat("fftMaxIntegral", &appImpl->fftMaxIntegralValue);
        ImGui::Separator();
        ImGui::DragFloat("hist scale", &histogram_scale, 0.001,0,0.1);
        auto width = ImGui::GetContentRegionAvailWidth();
        ImGui::PlotHistogram("", appImpl->fftDataIntegrated.data(), 750, 0, nullptr, 0.0f, appImpl->fftMaxIntegralValue, ImVec2(width,300));
        ImGui::PlotHistogram("", appImpl->fftData.data()          , 750, 0, nullptr, 0.0f, histogram_scale, ImVec2(width,300));
        ImGui::PlotHistogram("", appImpl->fftDataSmoothed.data()  , 750, 0, nullptr, 0.0f, histogram_scale, ImVec2(width,300));
        ImGui::Separator();

        ImGui::End();
    }

    bool selectInterpolation(tracker::Key& key) {
        bool selected = false;
        if (ImGui::BeginMenu("interpolation")) {
            if (ImGui::MenuItem("step")) {
                selected = true;
                key.interp.type = tracker::interpolation_type::Step;
            }
            if (ImGui::MenuItem("linear")) {
                key.interp.type = tracker::interpolation_type::Linear;
            }
            if (ImGui::MenuItem("smooth")) {
                key.interp.type = tracker::interpolation_type::Smooth;
            }
            if (ImGui::MenuItem("ramp")) {
                key.interp.type = tracker::interpolation_type::Ramp;
            }

            ImGui::EndMenu();
        }
        return selected;
    }

    void MasterNodeGui::drawTracker(bool *p_open) {
        if(!*p_open) return;
        if(!ImGui::Begin("Tracker", p_open)) {
            ImGui::End();
            return;
        }
        constexpr unsigned int LINE_COUNT = 7000;

        static bool track_active_row = true;
        ImGui::Checkbox("Track", &track_active_row);

        ImGui::PushItemWidth(100);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing,ImVec2(0,0));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,ImGuiCol_TitleBg);
        ImVec2 size_select(100, ImGui::GetTextLineHeight());
        ImGui::Text("row  ");
        for(auto& track : appImpl->tracks) {
            ImGui::SameLine();
            //TODO mute, fold?
            ImGui::Selectable(track.first.c_str(),false,0,size_select);
        }
        ImGui::BeginChild("TrackerTable");
        {
            ImGuiListClipper clip(LINE_COUNT);
            ImGui::BeginGroup();
            while(clip.Step()) {
                for(auto line = clip.DisplayStart; line < clip.DisplayEnd; line ++) {
                    ImGui::Text("%04d ", line);
                }
            }
            ImGui::EndGroup();
        }

        for(auto& [name, track] : appImpl->tracks) {
            ImGui::SameLine();
            ImGui::BeginGroup();
            //ImGui::Text(name.c_str());
            ImGuiListClipper clip(LINE_COUNT);
            ImGui::PushID(name.c_str());
            ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1,1,1,1));
            auto active_key = tracker::Key{};
            while(clip.Step()) {
                if(!track.keys.empty()) {
                    active_key = track.keys.at(track.get_lower_bound_position(clip.DisplayStart));
                }
                for(auto line = clip.DisplayStart; line < clip.DisplayEnd; line++) {
                    ImGui::PushID(line);
                    if(auto pos = track.get_exact_position(line)) {
                        auto& key = track.keys.at(*pos);
                        std::string label = std::to_string(key.value);
                        ImGui::PopStyleColor(1);
                        switch (key.interp.type) {
                            case tracker::interpolation_type::Step:
                                ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1,1,1,1));
                                break;
                            case tracker::interpolation_type::Linear:
                                ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1.000f, 0.430f, 0.350f, 1.000f));
                                break;
                            case tracker::interpolation_type::Smooth:
                                ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.000f, 1.000f, 0.000f, 1.000f));
                                break;
                            case tracker::interpolation_type::Ramp:
                                ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.000f, 0.618f, 1.000f, 1.000f));
                                break;
                        }

                        if(ImGui::Selectable(label.c_str(),false,0,size_select)) {
                            ImGui::OpenPopup("value_edit");
                        }
                        if(ImGui::IsItemHovered()) {
                            if(ImGui::IsKeyPressed(GLFW_KEY_I, false)) {
                                using tit = tracker::interpolation_type;
                                switch (key.interp.type) {
                                    case tit::Step: key.interp.type = tit::Linear;break;
                                    case tit::Linear: key.interp.type = tit::Smooth;break;
                                    case tit::Smooth: key.interp.type = tit::Ramp;break;
                                    case tit::Ramp: key.interp.type = tit::Step;break;
                                }
                            }
                            if(ImGui::IsKeyPressed(GLFW_KEY_DELETE, false)) {
                                track.delete_key(key.row);
                            }
                        }
                        if(ImGui::BeginPopup("value_edit")) {
                            ImGui::InputFloat(name.c_str(), &key.value);
                            selectInterpolation(track.keys.at(*pos));
                            ImGui::EndPopup();
                        }
//                        ImGui::InputFloat(label.c_str(), &track.keys.at(*pos).value);
                    } else {
                        if(ImGui::Selectable("---",false,0,size_select)) {
                            auto key = tracker::Key{};
                            key.row = line;
                            key.value = track.get_value(key.row);
                            track.set_key(std::move(key));
                        }
                    }
                    ImGui::PopID();
                }
            }
            ImGui::PopStyleColor(1);
            ImGui::PopID();
            ImGui::EndGroup();
        }
        ImGui::EndChild(); //TrackerTable
        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(2);
        ImGui::PopItemWidth();
//        ImGui::BeginChild("TrackerTable");
//        ImGui::Columns(column_count, "tracker_content");
//        ImGuiListClipper clipper(LINE_COUNT);  // Also demonstrate using the clipper for large list
//        while (clipper.Step())
//        {
//            for (unsigned int line = static_cast<unsigned int>(clipper.DisplayStart); line < clipper.DisplayEnd; line++) {
//                ImGui::Text("%d", line);
////                char label[32];
////                sprintf(label, "%04d", line);
//                //ImGui::Selectable(label, line == active_row);//, ImGuiSelectableFlags_SpanAllColumns);
//
//                if (track_active_row && line == active_row) {
//                    ImGui::SetScrollHere(0.5f); // 0.0f:top, 0.5f:center, 1.0f:bottom
//                }
//                ImGui::NextColumn();
//                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(0,0));
//                ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing,ImVec2(0,0));
//                ImGui::PushStyleColor(ImGuiCol_FrameBg,ImGuiCol_TitleBg);
//                for (auto &track : appImpl->tracks) {
//                    auto header = (track.first + "##" + std::to_string(line));
//                    if(auto pos = track.second.get_exact_position(line)) {
//                        ImGui::PushItemWidth(-1);
//                        auto& key = track.second.keys.at(*pos);
//                        ImGui::InputFloat(header.c_str(), &key.value);
//                        ImGui::PopItemWidth();
//                        if(ImGui::BeginPopupContextItem((header+"##edit").c_str())) {
//                            selectInterpolation(key);
//                            if(ImGui::SmallButton(("delete##" + header).c_str())){
//                                track.second.delete_key(line);
//                            }
//                            ImGui::EndPopup();
//                        }
//                    } else {
//                        float value = track.second.get_value(line);
//                        ImGui::PushItemWidth(-1);
//                        ImGui::InputFloat(header.c_str(), &value,0.0f,0.0f,-1,ImGuiInputTextFlags_ReadOnly);
//                        ImGui::PopItemWidth();
//                        //ImGui::TextDisabled("%f", track.second.get_value(line));
//                        if (ImGui::BeginPopupContextItem((header+"##new").c_str())) {
//                            auto key = tracker::Key{};
//                            key.value = 0;
//                            key.row = line;
//                            if(selectInterpolation(key)) {
//                                track.second.set_key(key);
//                            }
//                            ImGui::EndPopup();
//                        }
//                    }
//                    ImGui::NextColumn();
//
//                }
//                ImGui::PopStyleColor(1);
//                ImGui::PopStyleVar(2);
//            }
//        }
//
//        ImGui::Columns(1);
//        ImGui::Separator();
////        float scroll_y = ImGui::GetScrollY(), scroll_max_y = ImGui::GetScrollMaxY();
//        ImGui::EndChild();
//        //ImGui::Text("%.0f/%0.f", scroll_y, scroll_max_y);

        ImGui::End();

    }


}