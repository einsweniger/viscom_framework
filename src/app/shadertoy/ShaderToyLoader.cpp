//
// Created by bone on 15.04.18.
//

#include <cereal/archives/json.hpp>
#include <fstream>
#include <cassert>
#include "ShaderToyLoader.h"
#include "ShaderToy.h"

namespace shadertoy {

    ShaderToyLoader::ShaderToyLoader(fs::path path) {
        namespace fs = std::experimental::filesystem;

        auto file = std::ifstream(path);

        shadertoy::Shader toy;
        cereal::JSONInputArchive iarchive(file);
        iarchive(toy);
        toy_= std::make_unique<shadertoy::Shader>(toy);

        auto shaderResPath = fs::path("../resources/shader");
        outputfolder = shaderResPath / fs::path("shadertoy/" + toy_->info.id);
        fs::create_directory(outputfolder);
        for(auto& pass : toy_->renderpass) {
            if ("image" == pass.type) {
                assert(nullptr == image); //Shadertoys are assumed to only have one image
                image = std::make_unique<Renderpass>(pass);
            } else if ("buffer" == pass.type) {
                buffers.push_back(pass);
            } else if ("common" == pass.type) {
                assert(nullptr == common); //Shadertoys are assumed to only have one common shader
                common = std::make_unique<Renderpass>(pass);
            } else {
                std::cerr << pass.type << " is unsupported, sorry" << std::endl;
                continue; //skip file creation
            }
        }
        bool hasCommon = (nullptr != common);
        for(auto& pass : toy_->renderpass) {
            auto output =  outputfolder / fs::path(pass.name + ".frag");
            auto outstream = std::ofstream{ output};
            if("common" == pass.type){
                outstream << pass.code;
                continue;
            } else if ("buffer" == pass.type || "image" == pass.type) {
                outstream << VERSION
                          << "#include <../" + pass.type + "/header.glsl>\n"
                          << (hasCommon && "common" != pass.type ? "#include <Common.frag>\n" : "")
                          <<  pass.code
                          << "\n#include <../" + pass.type + "/footer.glsl>\n";
            }
        }

//        size_t counter =0;
//        for(auto& pass : toy_->renderpass) {
//            if("buffer" == pass.type) {
//                auto out = outputs.at(counter);
//                if(nullptr == active_fsq_) {
//                    active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>(out,this);
//                } else {
//                    active_fsq_->AddPass(out);
//                }
//            }
//            counter++;
//        }
//        counter = 0;
//        for(auto& pass : toy_->renderpass) {
//            if("image" == pass.type) {
//                auto out = outputs.at(counter);
//                if(nullptr == active_fsq_) {
//                    active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>(out,this);
//                } else {
//                    active_fsq_->AddPass(out);
//                }
//            }
//            counter++;
//        }
    }
}