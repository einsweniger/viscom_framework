/**
 * @file   CoordinatorNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the coordinator application node.
 */

#include <filesystem>

#include "CoordinatorNode.h"
#include <imgui.h>
#include <gui/gui.h>
#include <gui/dialogs/FileSelect.h>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <core/gfx/Shader.h>
#include "ToyPlayer.h"


namespace viscom {

    CoordinatorNode::CoordinatorNode(ApplicationNodeInternal* appNode) :
        ApplicationNodeImplementation{ appNode },
        appNode{appNode}
    {
        auto gentoys = GetConfig().resourceSearchPaths_.at(GetConfig().resourceSearchPaths_.size()-1);
        auto tmpfld = fs::path(gentoys);
        if(!fs::is_directory(tmpfld)) {
            fs::create_directory(tmpfld);
            std::cout << "creating shadertoy folder";
        }
    }

    CoordinatorNode::~CoordinatorNode() = default;

    void CoordinatorNode::Draw2D(FrameBuffer& fbo)
    {
        namespace fs = std::filesystem;
        static bool drawToyImport = true;
        static minuseins::gui::FileSelect select{"Import ShaderToy", GetConfig().resourceSearchPaths_, [this](fs::path path) {
            return loadShadertoy(path);
        }, "json/shadertoy/"};
        select.draw(&drawToyImport);


        ImGui::Begin("Imported Toys");
        if(nullptr != active_player) {
            active_player->draw2d();
        }
        ImGui::End();

        ApplicationNodeImplementation::Draw2D(fbo);
    }

    bool CoordinatorNode::loadShadertoy(fs::path path) {
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
            auto id = toy.info.id;
            auto toyp = std::make_unique<minuseins::ToyPlayer>(std::move(toy),this,this);
            toyp->init();
            active_player = std::move(toyp);
            //players.push_back();
        } catch (viscom::resource_loading_error& err) {
            std::cerr << err.errorDescription_ << std::endl;
        } catch (viscom::shader_compiler_error& err) {
            std::cerr << err.what() << std::endl;
        } catch (minuseins::toyplayer_exception& exception) {
            std::cerr << exception.message << std::endl;
        }
        return false;
    }

}
