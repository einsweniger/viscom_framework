//
// Created by -1 on 11/9/2017.
//

#include <app/gfx/gl/handlers.h>
#include <app/ApplicationNodeImplementation.h>
#include "ShaderToyFsq.h"


namespace minuseins {
    ShaderToyFsq::ShaderToyFsq(const std::string &fragmentShader) :
            IntrospectableFsq{fragmentShader}
    {
    }

    void ShaderToyFsq::init_hooks() {
        //IntrospectableFsq::init_hooks();

        auto& tm = appBase->GetTextureManager();
        for(auto& inp : params_.inputs) {
            if("texture" == inp.ctype) {
                auto tex = tm.GetResource(inp.src);
                usedTextures.push_back(tex);
                //TODO think about if input::sampler should be stored in other type than SamplerUniform
                //TODO if using double buffering, this needs to be communicated to sampler.
                uniformhdl->add_init_hook("iChannel" + std::to_string(inp.channel), [&](std::string_view name, generic_uniform* gu) {
                    auto& sampler = dynamic_cast<SamplerUniform&>(*gu);
                    auto x = tm.GetResource(inp.src);
                    auto texid = x->getTextureId();
                    sampler.boundTexture = texid;
                    if(inp.sampler.wrap == "repeat") {
                        gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_S, gl::GL_REPEAT);
                        gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_T, gl::GL_REPEAT);
                    } else if (inp.sampler.wrap == "mirror") { //GL_MIRRORED_REPEAT
                        gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_S, gl::GL_MIRRORED_REPEAT);
                        gl::glTextureParameteri(texid, gl::GL_TEXTURE_WRAP_T, gl::GL_MIRRORED_REPEAT);
                    } // clamp_to_edge is default here.
                    sampler.textureUnit = static_cast<GLint>(inp.channel);
                    if(inp.sampler.filter == "nearest") {
                        gl::glTextureParameteri(texid, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
                        gl::glTextureParameteri(texid, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);

                    } else if(inp.sampler.filter == "mipmap") {
                        std::cerr << "mipmap texture filter, implement?" << std::endl;
                    }
                });
                uniformhdl->add_init_hook("iChannelResolution[" + std::to_string(inp.channel) + "]", [&](std::string_view name, generic_uniform* gu) {
                    auto& floater = dynamic_cast<FloatUniform&>(*gu);
                    auto x = tm.GetResource(inp.src);
                    floater.value[0] = x->getDimensions().x;
                    floater.value[1] = x->getDimensions().y;
                    floater.value[2] = 1;
                });
            } else if ("buffer" == inp.ctype) {
                //TODO handle sampler w/buffer input.
            }
        } //end input handling
        for(auto& out : params_.outputs) {
            //TODO double buffer and hook.
            //out.channel
            //out.id <- if id not in inputs, no double buffering needed
        }
    }
}
