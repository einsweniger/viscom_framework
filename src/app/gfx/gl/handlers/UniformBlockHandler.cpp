//
// Created by bone on 24.04.18.
//

#include <app/gfx/gl/BasicInterface.h>
#include "UniformBlockHandler.h"
#include "UniformHandler.h"
#include <enh/ApplicationNodeBase.h>

namespace minuseins::handlers {

    std::unique_ptr<named_resource>
    UniformBlockHandler::initialize(ProgramInspector &inspect, named_resource res) {
        auto block = std::make_unique<UniformBlock>(std::move(res));
        block->buffer = std::make_unique<UniformBuffer>(block->name, block->buffer_data_size, appnode->GetUBOBindingPoints());
        appnode->GetUBOBindingPoints()->BindBufferBlock(inspect.programId_, block->name);
        return std::move(block);
    }

    void
    UniformBlockHandler::postInit(ProgramInspector &inspect, named_resource_container &resources) {
        auto interface = inspect.GetInterface(gl::GL_UNIFORM_BLOCK);
        try {
            auto& uniforms = inspect.GetContainer(gl::GL_UNIFORM);
            for(auto& res : resources) {
                auto& block = dynamic_cast<UniformBlock&>(*res);
                auto active_vars = interface.GetActiveVariables(block.resourceIndex, block.num_active_variables);
                for(auto& resIndex : active_vars) {
                    auto& uniform = dynamic_cast<generic_uniform&>(*uniforms.at(resIndex));
                    uniform.uploadfn = [&](generic_uniform& self){
                        auto offset = self.properties.at(gl::GL_OFFSET);
                        block.buffer->UploadData(offset, self.uploadSize(), self.valuePtr());
                    };
                }
            }
        } catch (std::out_of_range& err) {
            std::cerr << err.what() << std::endl;
        }

    }

    void
    UniformBlockHandler::prepareDraw(ProgramInspector &inspect, named_resource_container &resources) {
        for(auto& res : resources) {
            auto& block = dynamic_cast<UniformBlock&>(*res);
            block.buffer->BindBuffer();
        }
    }

    UniformBlock::UniformBlock(named_resource res) :
            named_resource(std::move(res)),
            buffer_binding{static_cast<gl::GLuint>(properties.at(gl::GL_BUFFER_BINDING))},
            buffer_data_size{static_cast<gl::GLuint>(properties.at(gl::GL_BUFFER_DATA_SIZE))},
            num_active_variables{static_cast<gl::GLuint>(properties.at(gl::GL_NUM_ACTIVE_VARIABLES))}
    {
    }
}
