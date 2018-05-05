//
// Created by bone on 30.04.18.
//

#include "ShaderToySamplerBuilder.h"
#include <enh/ApplicationNodeBase.h>
#include <app/shadertoy/ShaderToy.h>

namespace minuseins::handlers {

    namespace detail {

        iChannel::iChannel(named_resource res, const shadertoy::Input &input, viscom::enh::ApplicationNodeBase *appBase)
                : generic_uniform(std::move(res)), input(input), appBase(appBase)
        {
            uname = "iChannel" + std::to_string(input.channel);
        }

        void iChannel::init(gl::GLuint program) {
            tex = appBase->GetTextureManager().GetResource(input.src);
            auto texid = tex->getTextureId();
            if(input.sampler.wrap == "repeat") {
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_S, gl::GL_REPEAT);
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_T, gl::GL_REPEAT);
            } else if (input.sampler.wrap == "mirror") { //GL_MIRRORED_REPEAT
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_S, gl::GL_MIRRORED_REPEAT);
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_T, gl::GL_MIRRORED_REPEAT);
            } // clamp_to_edge is default here.

            if(input.sampler.filter == "nearest") {
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
            } else if(input.sampler.filter == "mipmap") {
                //std::cerr << "mipmap texture filter, implement?" << std::endl;
            }
        }

        bool iChannel::upload_value() {
            if(do_upload) {
                gl::glActiveTexture(gl::GL_TEXTURE0 + input.channel);
                gl::glBindTexture(gl::GL_TEXTURE_2D, tex->getTextureId());
                gl::glUniform1i(location(), input.channel);
            }
            return true;
        }

        struct iChannelResolution : FloatUniform {
            iChannelResolution(named_resource res, viscom::enh::ApplicationNodeBase *appBase, const shadertoy::Input &input) :
                    FloatUniform(std::move(res)),
                    appBase(appBase),
                    input(input)
            {
                uname = "iChannelResolution["+std::to_string(input.channel)+"]";
            }

            viscom::enh::ApplicationNodeBase* appBase;
            shadertoy::Input input;
            std::shared_ptr<viscom::Texture> tex;
            std::string uname;

            void init(gl::GLuint program) override {
                tex = appBase->GetTextureManager().GetResource(input.src);
                value[0] = tex->getDimensions().x;
                value[1] = tex->getDimensions().y;
                value[2] = 1;
            }
        };
    }


    ShaderToySamplerBuilder::ShaderToySamplerBuilder(viscom::enh::ApplicationNodeBase *appBase, const shadertoy::Renderpass &pass)
            : ExternalUniformBuilder(appBase), pass(pass)
    {
        for(auto& inp : pass.inputs) {
            if("texture" == inp.ctype) {
                //TODO think about if input::sampler should be stored in other type than SamplerUniform
                //TODO if using double buffering, this needs to be communicated to sampler.

            } else if ("buffer" == inp.ctype) {
                //TODO handle sampler w/buffer input.
            }
        } //end input handling
        for(auto& out : pass.outputs) {
            //TODO double buffer and hook.
            //out.channel
            //out.id <- if id not in inputs, no double buffering needed
        }
    }

    std::unique_ptr<generic_uniform> ShaderToySamplerBuilder::operator()(named_resource res) {
        if("tex_text" == res.name && res.properties.at(gl::GL_TYPE) == gl::GL_SAMPLER_2D) {
            auto inp = shadertoy::Input{};
            inp.src = "/media/a/text.png";
            inp.sampler.wrap = "repeat";
            inp.channel = 0;
            return std::make_unique<detail::iChannel>(std::move(res), std::move(inp), appBase);
        }

        if(res.name.length() == 9)  { //ichannel0
            if(res.properties.at(gl::GL_TYPE) == gl::GL_SAMPLER_2D) {
                auto substr = res.name.substr(0, 8);
                std::cout << "sampler with substring:" << substr << "followed by " << res.name.substr(8, 1) << std::endl;
                int channel = atoi(res.name.substr(8, 1).c_str());
                std::cout << "on channel:" << std::to_string(channel) << std::endl;
                if(substr == "iChannel") {
                    auto it = std::find_if(std::begin(pass.inputs), std::end(pass.inputs),
                                           [&](shadertoy::Input &input) {
                                               return input.channel == channel;
                                           });
                    if (std::end(pass.inputs) != it) {
                        return std::make_unique<detail::iChannel>(std::move(res), (*it), appBase);
                    }

                }
            }
        }

        return ExternalUniformBuilder::operator()(std::move(res));
    }

}