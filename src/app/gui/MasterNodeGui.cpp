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
                ImGui::MenuItem("NewScene", "", &activeWindows["NewScene"]);
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
            ImGui::SliderFloat("##Time", &appImpl->currentTime_,0.0f, 300.f);
            if(ImGui::IsItemActive()) {
                appImpl->bass->set_position(appImpl->currentTime_);
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
        ImGui::PlotHistogram("", appImpl->fftDataIntegrated.data(), 750, 0, nullptr, 0.0f, 4*1024.0f,       ImVec2(width,300));
        ImGui::PlotHistogram("", appImpl->fftData.data()          , 750, 0, nullptr, 0.0f, histogram_scale, ImVec2(width,300));
        ImGui::PlotHistogram("", appImpl->fftDataSmoothed.data()  , 750, 0, nullptr, 0.0f, histogram_scale, ImVec2(width,300));
        ImGui::Separator();

        ImGui::End();
    }


}