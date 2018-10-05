//
// Created by bone on 05.10.18.
//

#ifndef PROJECTORFRAMEWORK_BUFFERPASS_H
#define PROJECTORFRAMEWORK_BUFFERPASS_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <Shadertoy.h>
#include <core/gfx/FrameBuffer.h>

namespace viscom {
    class Texture;
    class ApplicationNodeImplementation;
    class ApplicationNodeBase;
    class GPUProgram;
    class FullscreenQuad;
}
namespace minuseins {
    class ToyPlayer;
    class ProgramInspector;
}
namespace player {

    struct BufferPass {
        BufferPass(viscom::ApplicationNodeBase *appBase, shadertoy::Renderpass params, const std::string &shadertoy_id);

        //initialized by constructor
        viscom::ApplicationNodeBase* appBase;
        viscom::ApplicationNodeImplementation* appImpl;
        std::unique_ptr<shadertoy::Renderpass> params;
        std::string shadertoy_id;

        void writeShaders(bool hasCommonPass, minuseins::ToyPlayer* toy);
        //initialized during writeShaders
        std::unordered_map<std::string, std::shared_ptr<viscom::Texture>> textures;
        std::unique_ptr<viscom::FullscreenQuad> fsq;
        std::unique_ptr<minuseins::ProgramInspector> inspect;
        std::vector<viscom::FrameBuffer> pingbuffer;
        std::vector<viscom::FrameBuffer> pongbuffer;

        void drawToBackBuffer();
        const unsigned int & getOutputTextureId();
        void WriteBuffer(viscom::function_view<void()> drawfn);
        bool lastWriteWasPong = false;

        void draw2d();
        void ClearBuffer();
    };
}


#endif //PROJECTORFRAMEWORK_BUFFERPASS_H
