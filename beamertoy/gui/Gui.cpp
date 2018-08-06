//
//
// Created by bone on 15.04.18.
//

#include "Gui.h"

#include <fstream>

#include <glbinding/glbinding.h>
#include <glbinding/gl/types.h>
#include <glbinding/gl/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <core/glfw.h>
#include <core/resources/Resource.h>
#include <core/resources/ResourceManager.h>
#include <core/gfx/Shader.h>
#include <core/gfx/FrameBuffer.h>
#include <cereal/archives/json.hpp>

#include "../app/ApplicationNodeImplementation.h"
#include "dialogs/FileSelect.h"
#include <Shadertoy.h>


namespace minuseins::gui {

    //static Overlay overlay;

    Gui::Gui(viscom::ApplicationNodeImplementation *appImpl, viscom::ApplicationNodeInternal* appNode) :
            appImpl{appImpl},
            appNode{appNode}
    {
        try {
//            auto config_file = viscom::Resource::FindResourceLocation(config_name, appNode);

        } catch (viscom::resource_loading_error&) {
            activeWindows["MainMenu"] = true;
        }
        activeWindows["MainMenu"] = true;
        //TODO use vector to draw menu and toggles.
        auto searchPaths = appNode->GetFramework().GetConfig().resourceSearchPaths_;
    }

    void Gui::init() {
        {  //set error callbacks
            using namespace glbinding;
            setCallbackMaskExcept(CallbackMask::After | CallbackMask::ParametersAndReturnValue, { "glGetError" });
            //setAfterCallback(std::bind(&OglLog::callback, &log, std::placeholders::_1));
        }
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    }

    void Gui::cleanup()
    {
        {  //set error callbacks
            using namespace glbinding;
            glbinding::removeCallbackMask(CallbackMask::After | CallbackMask::ParametersAndReturnValue);
        }
    }

    void Gui::UpdateFrame(double currentTime, double elapsedTime) {
        //overlay.UpdateFrame(currentTime, elapsedTime);
    }

    void Gui::Draw2D(viscom::FrameBuffer &fbo) {
        fbo.DrawToFBO([this]() {
            //overlay.drawOverlay(&activeWindows["Overlay"]);
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

            //log.Draw("OGLerrors", &activeWindows["OGLerrors"]);
            //log.Clear();
            drawToyInfo();
        });
    }

    void Gui::drawMainMenu(bool *p_open) {
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
//                ImGui::MenuItem("StopTime", "SPACE", &appImpl->stopTime_);
//                ImGui::MenuItem("DrawToy", "", &appImpl->drawToy);
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

    void Gui::toggle(std::string item) {
        activeWindows[item] = !activeWindows[item];
    }



    void Gui::drawShaderToySelectImport(bool *p_open) {
        if(!*p_open) return;
        static FileSelect select{"Import ShaderToy",appNode->GetFramework().GetConfig().resourceSearchPaths_, [this](fs::path path) {
            return ShaderToyCallback(path);
        }, "json/shadertoy/"};
        select.draw(p_open);
    }
    void Gui::drawShaderWindow(bool *p_open) {
        if(!*p_open) return;
        if(!ImGui::Begin("Shaders", p_open)){
            ImGui::End();
            return;
        }

        ImGui::End();
    }

    void Gui::drawShaderImport(bool *p_open) {
        if(!*p_open) return;
        static auto staged_shaders = std::vector<std::string>{};
        static std::string progname{};
        progname.reserve(50);
        //static auto& gm = appNode->GetGPUProgramManager();
        static FileSelect shader_select("Open Shader",appNode->GetFramework().GetConfig().resourceSearchPaths_, [&](fs::path path) {
            try {
//                auto fsq = std::make_unique<IntrospectableFsq>(path.string());
//                fsq->init(appImpl);
//                appImpl->fsqs.push_back(std::move(fsq));
                return false;
            } catch (viscom::shader_compiler_error &err) {
                std::cout << err.what() << std::endl;
                return false;
            }
        }, "shader/");
        shader_select.draw(p_open);
    }

    void Gui::drawTextureImportWindow(bool *p_open) {
        if(!*p_open) return;
        static FileSelect texture_select{"Open Texture",appNode->GetFramework().GetConfig().resourceSearchPaths_, [&](fs::path path) {
            return textureCallback(path);
        }};
        texture_select.draw(p_open);
    }

    void Gui::drawTextureWindow(bool *p_open) {
        if(!*p_open) return;
        static viscom::ResourceManager<viscom::Texture>& tm = appNode->GetFramework().GetTextureManager();
        if(!ImGui::Begin("Textures", p_open)) {
            ImGui::End();
            return;
        }

//        std::for_each(tm.cbegin(), tm.cend(), [](const auto& tex) {
//            if(!tex.second.expired()) {
//                std::shared_ptr<viscom::Texture> texture = tex.second.lock();
//                ImGui::Text("texid: %d", texture->getTextureId());
//                ImGui::SameLine();
//                ImGui::TextUnformatted(tex.first.c_str());
//                ImVec2 uv0(0, 1);
//                ImVec2 uv1(1, 0);
//                auto maxwidth = ImGui::GetContentRegionAvailWidth();
//                auto texdim = texture->getDimensions();
//                float aspect = texdim.x / texdim.y;
//                if(aspect > 1.0) {aspect = 1.0f/aspect;};
//                ImVec2 region(maxwidth, maxwidth * aspect);
//                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) texture->getTextureId()), region, uv0, uv1);
//            }
//        });

        ImGui::End();
    }

    bool Gui::textureCallback(fs::path path) {
        static auto& tm = appNode->GetFramework().GetTextureManager();
        try {
//            openTextures.push_back(tm.GetResource(path.string()));
            return !appNode->GetFramework().IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
        } catch (viscom::resource_loading_error &err) {
            std::cout << err.errorDescription_ << std::endl;
            return false;
        }
    }


    bool Gui::ShaderToyCallback(fs::path path) {
        //TODO clean loading!
        try {
            std::cout << path.string() << std::endl;
            if(!path.has_extension())
                return false;
            if(".json" != path.extension())
                return false;
            shadertoy::Shader toy;
            auto file = std::ifstream(path);
            cereal::JSONInputArchive iarchive(file);

            iarchive(toy);
            appImpl->toys.push_back(std::make_unique<shadertoy::Shader>(toy));
            //std::cout << toy.renderpass[0].code << std::endl;
            auto quad_ifs = std::ifstream(fs::path{"../extern/fwcore/resources/shader/fullScreenQuad.vert"});
            std::string content( (std::istreambuf_iterator<char>(quad_ifs)), (std::istreambuf_iterator<char>()));
            //std::cout << content << std::endl;
            auto& progman = appNode->GetFramework().GetGPUProgramManager();
            auto toypath = fs::path{"../resources/shader/tmptoy.frag"};
            std::ofstream ofst{toypath};
            ofst << toy.renderpass[0].code;
            ofst.close();
            //std::vector<std::string> progs = {content, toy.renderpass[0].code};
            auto prog = progman.GetResource("test_toy", std::vector<std::string>{"fullScreenQuad.vert", "tmptoy.frag"});
//                loader = std::make_unique<shadertoy::ShaderToyLoader>(realpath);
//                //appImpl->fsqs.clear();
//                //TODO Renderpass.name is sometimes empty
//                for(auto& buf : loader->buffers) {
//                    std::cout << buf.name << std::endl;
//                    auto outfile = fs::path{"shadertoy/"+loader->toy_->info.id +"/"+ buf.name + ".frag"};
//                    auto tq = std::make_unique<minuseins::ShaderToyFsq>(outfile.string());
//                    tq->loadParams(buf);
//                    tq->init(appImpl);
//
//                    appImpl->toys.push_back(std::move(tq));
//                }
//                auto outfile = fs::path{"shadertoy/"+loader->toy_->info.id +"/"+ loader->image.name + ".frag"};
//                auto tq = std::make_unique<minuseins::ShaderToyFsq>(outfile.string());
//                tq->loadParams(loader->image);
//                tq->init(appImpl);
//
//                appImpl->toys.push_back(std::move(tq));
//                return false;
//            } else { return false; }
            return false;
        } catch (viscom::resource_loading_error& err) {
            std::cerr << err.errorDescription_ << std::endl;
            return false;
        } catch (viscom::shader_compiler_error& err) {
            std::cerr << err.what() << std::endl;
            return false;
        }

    }

    void Gui::drawGPUProgram(bool *p_open) {
        if(!*p_open) return;
        {
//            auto& fsqs = appImpl->fsqs;
//            for (auto it = fsqs.begin(); it != fsqs.end();) {
//                ImGui::PushID((*it)->fragmentShader.c_str());
//                ImGui::Begin("GPUProgram");
//                std::string buttonText = "x##"+(*it)->fragmentShader;
//                if(ImGui::SmallButton(buttonText.c_str())) {
//                    it = fsqs.erase(it);
//                } else {
//                    ImGui::SameLine();
//                    (*it)->Draw2D(p_open);
//                    ++it;
//                }
//
//                ImGui::End();
//                ImGui::PopID();
//            }
        }

//        auto& toys = appImpl->toys;
//        for (auto it = toys.begin(); it != toys.end();) {
//            ImGui::PushID((*it)->fragmentShader.c_str());
//            ImGui::Begin("ShaderToys");
//            std::string buttonText = "x##"+(*it)->fragmentShader;
//            if(ImGui::SmallButton(buttonText.c_str())) {
//                it = toys.erase(it);
//            } else {
//                ImGui::SameLine();
//                (*it)->Draw2D(p_open);
//                ++it;
//            }
//
//            ImGui::End();
//            ImGui::PopID();
//        }

    }

    void Gui::drawTimeSlider(bool *p_open) {
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
//            ImGui::DragInt("##Time", reinterpret_cast<int *>(&appImpl->currentRow), 1, 0,7000);
//            if(ImGui::IsItemActive()) {
//                appImpl->bass->set_row(appImpl->currentRow);
//            }
            ImGui::PopItemWidth();
        }

        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    void Gui::drawAnimation(bool *p_open) {
        if(!*p_open) return;
//        static gui::Animation anim(appNode->GetConfig().resourceSearchPaths_.at(0), appNode->GetCamera());
//        anim.draw(p_open);
    }

    void Gui::drawGlobalVars(bool *p_open) {
        static float histogram_scale = 0.07;
        if(!*p_open) return;
        if(!ImGui::Begin("Global Variables")) {
            ImGui::End();
            return;
        }
        auto o = appImpl->GetCamera()->GetOrientation();

        ImGui::InputFloat4("cam orientation", glm::value_ptr(o));

//        ImGui::DragFloat("fftSmoothing",&appImpl->fftSmootingFactor, 0.01f,0.0f,0.98f);
//        ImGui::DragFloat("fftMaxIntegral", &appImpl->fftMaxIntegralValue);
//        ImGui::Separator();
//        ImGui::DragFloat("hist scale", &histogram_scale, 0.001,0,0.1);
//        auto width = ImGui::GetContentRegionAvailWidth();
//        ImGui::PlotHistogram("", appImpl->fftDataIntegrated.data(), 750, 0, nullptr, 0.0f, appImpl->fftMaxIntegralValue, ImVec2(width,300));
//        ImGui::PlotHistogram("", appImpl->fftData.data()          , 750, 0, nullptr, 0.0f, histogram_scale, ImVec2(width,300));
//        ImGui::PlotHistogram("", appImpl->fftDataSmoothed.data()  , 750, 0, nullptr, 0.0f, histogram_scale, ImVec2(width,300));
        ImGui::Separator();

        ImGui::End();
    }

    void Gui::drawTracker(bool *p_open) {
//        static gui::Tracker tracker(appImpl);
//        tracker.Draw(p_open);
    }

    void Gui::drawToyInfo() {
        static std::map<std::string, std::shared_ptr<viscom::Texture>> textures;
        for(auto& toy : appImpl->toys) {
            ImGui::Begin(toy->info.id.c_str());
            if(ImGui::TreeNode("general:")) {
                ImGui::TextUnformatted(toy->info.id.c_str());
                ImGui::TextUnformatted(toy->info.name.c_str());
                ImGui::TreePop();
            }
            auto childsize = ImVec2(ImGui::GetWindowContentRegionWidth() / toy->renderpass.size(), 0);
            bool first = true;
            for(auto& pass : toy->renderpass) {
                if(!first) {
                    ImGui::SameLine(0,0);
                } else {
                    first = false;
                }
                ImGui::PushID(pass.name.c_str());
                ImGui::BeginChild(pass.name.c_str(), childsize, true); //, ImGuiWindowFlags_MenuBar);
                ImGui::Text("name: %s",pass.name.c_str());

                ImGui::InputTextMultiline("code:",&pass.code[0], pass.code.length(), ImVec2(ImGui::GetContentRegionAvailWidth(),0));

                ImGui::Text("type: %s", pass.type.c_str());
                ImGui::TextUnformatted("inputs:");
                for(auto& inp : pass.inputs) {
                    ImGui::Text("id: %d, ctype: %s, src: %s, channel: %d", inp.id, inp.ctype.c_str(), inp.src.c_str(), inp.channel);
                }
                ImGui::TextUnformatted("outputs:");
                for(auto& out : pass.outputs) {
                    ImGui::Text("id: %d, channel: %d", out.id, out.channel);
                }

                ImGui::BeginChild("inputs");
                bool firstTex=true;
                ImVec2 uv0(0, 1);
                ImVec2 uv1(1, 0);
                auto availWidth = ImGui::GetContentRegionAvailWidth() / 4;
                for(auto& inp : pass.inputs) {
                    //TODO channels are not sorted by inp.id, appear in wrong order.
                    if(firstTex) {
                        firstTex = false;
                    } else {
                        ImGui::SameLine(0,0);
                    }
                    try {
                        auto id = textures.at(inp.src)->getTextureId();
                        ImVec2 region(availWidth, availWidth / 1.7f);
                        ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) id), region, uv0, uv1);

                    } catch (std::out_of_range& ) {
                        textures[inp.src] = appNode->GetFramework().GetTextureManager().GetResource(inp.src);
                    }
                }
                ImGui::EndChild();
                ImGui::EndChild();
                ImGui::PopID();
            }
            ImGui::End();
        }
    }


}