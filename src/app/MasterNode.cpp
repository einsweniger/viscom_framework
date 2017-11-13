/**
 * @file   MasterNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the master application node.
 */

#include "MasterNode.h"
#include "core/glfw.h"
#include <imgui.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glbinding/gl/gl.h>
#include "../../extern/fwcore/src/core/gfx/Shader.h"


void checkGlError() {
    
    while (true) {
        const auto err = gl::glGetError();
        switch (err) {
        case gl::GL_INVALID_ENUM:
                printf("GLerror: invalid enum\n");
                break;
            case gl::GL_INVALID_VALUE:
                printf("GLerror: invalid value\n");
                break;
            case gl::GL_INVALID_OPERATION:
                printf("GLerror: invalid operation\n");
                break;
            case gl::GL_STACK_OVERFLOW:
                printf("GLerror: stack overflow\n");
                break;
            case gl::GL_STACK_UNDERFLOW:
                printf("GLerror: stack underflow\n");
                break;
            case gl::GL_OUT_OF_MEMORY:
                printf("GLerror: out of memory\n");
                break;
            case gl::GL_INVALID_FRAMEBUFFER_OPERATION:
                printf("GLerror: invalid framebuffer operation\n");
                break;
            case gl::GL_CONTEXT_LOST:
                printf("GLerror: context lost\n");
                break;
            case gl::GL_TABLE_TOO_LARGE:
                printf("GLerror: table too large\n");
                break;
            case gl::GL_NO_ERROR:
                printf("GLerror: no error\n");
                return;
            default:
                printf("GLerror: wtf\n");
                break;
        }
    }
}
struct ExampleAppLog
{
    ImGuiTextBuffer Buf;
    ImVector<int> LineOffsets;        // Index to lines offset
    bool ScrollToBottom = false;

    void Clear()
    {
        Buf.clear();
        LineOffsets.clear();
    }

    void AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
            va_start(args, fmt);
        Buf.appendv(fmt, args);
            va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++) {
            if (Buf[old_size] == '\n') {
                LineOffsets.push_back(old_size);
            }
        }
        ScrollToBottom = true;
    }

    void Draw(const char* title, bool* p_open = nullptr, const std::function<void()>& drawFn = nullptr)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin(title, p_open);
        drawFn();
        ImGui::SameLine();
        if (ImGui::Button("Clear")) { Clear(); }
//        ImGui::SameLine();
//        bool copy = ImGui::Button("Copy");
//        ImGui::SameLine();
        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
//        if (copy) { ImGui::LogToClipboard(); }

        ImGui::TextUnformatted(Buf.begin());

        if (ScrollToBottom) {
            ImGui::SetScrollHere(1.0f);
        }
        ScrollToBottom = false;
        ImGui::EndChild();
        ImGui::End();
    }
};

namespace viscom {

    MasterNode::MasterNode(ApplicationNodeInternal* appNode) :
        ApplicationNodeImplementation{appNode}
    {
    }

    MasterNode::~MasterNode() = default;

    void drawOverlay(bool* p_open, double timeDelta)
    {
        static int histIdx = 0;
        static float histData[90] = { 0 };
        histData[histIdx] = static_cast<float>(timeDelta*1000);
        histIdx = (1 + histIdx) % 90;
        ImGui::SetNextWindowPos(ImVec2(10,20));
        if (!ImGui::Begin("Example: Fixed Overlay", p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
        {
            ImGui::End();
            return;
        }
        ImGui::Text("Draw Time (ms): %.2f", timeDelta*1000);
        ImGui::PlotHistogram("", histData, 90, 0, NULL, 0.0f, 60.0f, ImVec2(0,90));
        ImGui::Separator();
        ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        ImGui::End();
    }
    void drawCameraWindow(CameraHelper* pHelper, bool* p_open)
    {
        ImGui::Begin("Camera Params", p_open);
        glm::vec3 userPos = pHelper->GetPosition();

        ImGui::InputFloat3("user position", glm::value_ptr(userPos));
        ImGui::End();

        auto camQuat = pHelper->GetOrientation();

        if (ImGui::Begin("Camera Params")) {

            ImGui::InputFloat4("cam quat", glm::value_ptr(camQuat));
        }
        ImGui::End();


    }
    void MasterNode::drawProgramWindow(bool* p_open)
    {
        using enum2enums = std::pair<gl::GLenum, std::vector<gl::GLenum>>;
        using enum2str = std::pair<gl::GLenum, std::vector<gl::GLenum>>;
        const std::unordered_map<gl::GLenum, std::vector<gl::GLenum>> resourceProperties {
            {gl::GL_UNIFORM,         {gl::GL_NAME_LENGTH, gl::GL_ARRAY_SIZE, gl::GL_BLOCK_INDEX, gl::GL_LOCATION}},  // ARRAY_STRIDE , IS_ROW_MAJOR , MATRIX_STRIDE, ATOMIC_COUNTER_BUFFER_INDEX
            {gl::GL_UNIFORM_BLOCK,   {gl::GL_NAME_LENGTH, gl::GL_ACTIVE_VARIABLES, gl::GL_BUFFER_BINDING, gl::GL_NUM_ACTIVE_VARIABLES}},  // BUFFER_DATA_SIZE
            {gl::GL_PROGRAM_INPUT,   {gl::GL_NAME_LENGTH, gl::GL_ARRAY_SIZE, gl::GL_IS_PER_PATCH, gl::GL_LOCATION, gl::GL_LOCATION_COMPONENT}},
            {gl::GL_PROGRAM_OUTPUT,  {gl::GL_NAME_LENGTH, gl::GL_ARRAY_SIZE, gl::GL_IS_PER_PATCH, gl::GL_LOCATION, gl::GL_LOCATION_COMPONENT, gl::GL_LOCATION_INDEX}},
            {gl::GL_BUFFER_VARIABLE, {gl::GL_NAME_LENGTH, gl::GL_ARRAY_SIZE}},  // ARRAY_STRIDE , BLOCK_INDEX , IS_ROW_MAJOR , MATRIX_STRIDE
        };

        const std::unordered_map<gl::GLenum, std::string> interfaces {
            {gl::GL_UNIFORM, "GL_UNIFORM"},
            {gl::GL_UNIFORM_BLOCK, "GL_UNIFORM_BLOCK"},
            {gl::GL_PROGRAM_INPUT, "GL_PROGRAM_INPUT"},
            {gl::GL_PROGRAM_OUTPUT, "GL_PROGRAM_OUTPUT"},
            {gl::GL_BUFFER_VARIABLE, "GL_BUFFER_VARIABLE"},
//            {gl::GL_ATOMIC_COUNTER_BUFFER, "GL_ATOMIC_COUNTER_BUFFER"},
//            {gl::GL_SHADER_STORAGE_BLOCK, "GL_SHADER_STORAGE_BLOCK"}
        };
        const std::unordered_map<gl::GLenum, std::string> interfaceProperties {
            {gl::GL_ACTIVE_RESOURCES, "GL_ACTIVE_RESOURCES"},
            {gl::GL_MAX_NAME_LENGTH, "GL_MAX_NAME_LENGTH"},  // not for ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER
//            {gl::GL_MAX_NUM_ACTIVE_VARIABLES, "GL_MAX_NUM_ACTIVE_VARIABLES"},  // only for ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK
//            {gl::GL_MAX_NUM_COMPATIBLE_SUBROUTINES, "GL_MAX_NUM_COMPATIBLE_SUBROUTINES"}  // only for {VERTEX,GEOMETRY,FRAGMENT,COMPUTE}_SUBROUTINE_UNIFORM , TESS_{CONTROL,EVALUATION}_SUBROUTINE_UNIFORM
        };
        const std::unordered_map<gl::GLenum, std::string> programStageProperties {
            {gl::GL_ACTIVE_SUBROUTINE_UNIFORMS,           "GL_ACTIVE_SUBROUTINE_UNIFORMS"},
            {gl::GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,  "GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS"},
            {gl::GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH, "GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH"},
            {gl::GL_ACTIVE_SUBROUTINES,                   "GL_ACTIVE_SUBROUTINES"},
            {gl::GL_ACTIVE_SUBROUTINE_MAX_LENGTH,         "GL_ACTIVE_SUBROUTINE_MAX_LENGTH"},
        };

        auto prog = quad_->GetGPUProgram();
        GLuint id = prog->getProgramId();
        if(ImGui::Begin("GPUProgram", p_open)) {
            ImGui::BulletText("Program: %d", id);
            for(auto interface : interfaces) {
                if(ImGui::TreeNode(interface.second.c_str())){
                    for (auto property : interfaceProperties) {
                        GLint value;
                        gl::glGetProgramInterfaceiv(id, interface.first, property.first, &value);
                        ImGui::Text("%s: %d", property.second.c_str(), value);
                        value = 0;  // you have to reset the value by yourself, because GLFW won't.
                    }
                    ImGui::TreePop();
                }
            }
            if(ImGui::TreeNode("uniform locations")) {
                for(const auto& uniform : quad_->GetUniforms()) {
                    ImGui::Text("%s: %d", uniform.first.c_str(), uniform.second);
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Subroutine details")) {
                GLint maxSubRoutines,maxSubroutineUniformLocations;
                gl::glGetIntegerv(gl::GL_MAX_SUBROUTINES, &maxSubRoutines);
                gl::glGetIntegerv(gl::GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubroutineUniformLocations);
                ImGui::Text("GL_MAX_SUBROUTINES: %d", maxSubRoutines);
                ImGui::Text("GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS: %d", maxSubroutineUniformLocations);
                for(auto progStageProp : programStageProperties) {
                    GLint value;
                    gl::glGetProgramStageiv(id, gl::GL_FRAGMENT_SHADER, progStageProp.first, &value);
                    ImGui::Text("%s: %d", progStageProp.second.c_str(), value);
                }

                for(const auto& uniform : quad_->GetSubroutineUniforms()) {
                    GLint activeSubroutine = gl::glGetSubroutineUniformLocation(id, gl::GL_FRAGMENT_SHADER, &uniform.first[0]);
                    ImGui::Text("uniform %d: %s (active sub: %d)", uniform.second, uniform.first.c_str(), activeSubroutine);
                    for(const auto& subroutine : quad_->GetSubroutineCompatibleUniforms(uniform.second)) {
                        ImGui::BulletText("subroutine %d: %s", subroutine.second, subroutine.first.c_str());
                    }
                }
                ImGui::TreePop();
            }


        }
        ImGui::End();

    }

    void MasterNode::drawMainMenu(const bool* p_open) {
        if(p_open && ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("Scene")) {
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Shader")) {
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows")) {
                ImGui::MenuItem("Buffers", "B", &imBuffersWindow_);
                ImGui::MenuItem("Menu", "M", &imMainMenu_);
                ImGui::MenuItem("Overlay", "O", &imOverlay_);
                ImGui::MenuItem("Shader", "Ctrl+S", &imShaderWindow_);
                ImGui::MenuItem("GPUProgram","G", &imProgramRecourceWindow_);
                ImGui::Separator();
                ImGui::MenuItem("ImGui Demo", "", &imDemoWindow_);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void MasterNode::Draw2D(FrameBuffer& fbo)
    {
        static ExampleAppLog log;
        static bool show_camera_window;

        fbo.DrawToFBO([this]() {
            if(imDemoWindow_) ImGui::ShowTestWindow();
            if(imMainMenu_) drawMainMenu(&imMainMenu_);
            if(imOverlay_) drawOverlay(&imOverlay_, GetTimeDelta());

            if (imShaderWindow_) {
                log.Draw("Shader Reloading", &imShaderWindow_, [this]() {
                    if (ImGui::Button("recompile shaders")) {
                        try {
                            GetGPUProgramManager().RecompileAll();
                            log.AddLog("reload succesful\n");
                        } catch (shader_compiler_error& compilerError) {
                            log.AddLog(compilerError.what());
                        }
                    }
                });
            }

            if(show_camera_window) {
                auto cam = GetApplication()->GetCamera();
                drawCameraWindow(cam, &show_camera_window);
            }
            if (imBuffersWindow_) {
                if(ImGui::Begin("Buffers:", &imBuffersWindow_)) {
                    std::string name{ "tex " };
                    for (auto tex : SelectOffscreenBuffer(GetDebugTextureBuffer())->GetTextures()) {
                        std::string headerName = name + std::to_string(tex);
                        if (ImGui::CollapsingHeader(headerName.c_str())) {
                            ImVec2 uv0(0, 1);
                            ImVec2 uv1(1, 0);
                            ImVec2 region(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionWidth() / 1.7f);
                            ImGui::Image(reinterpret_cast<ImTextureID>(tex), region, uv0, uv1);
                        };
                    }
                }
                ImGui::End();
            }
            if(imProgramRecourceWindow_) drawProgramWindow(&imProgramRecourceWindow_);
        });
    }
    bool MasterNode::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        if (ApplicationNodeImplementation::KeyboardCallback(key, scancode, action, mods)) {return true;}

        if(!(action == GLFW_PRESS || action == GLFW_REPEAT)) {
            return false;
        }

        switch (key) {
            default: return false;
            case GLFW_KEY_TAB: toggleMouseGrab(); return true;
            case GLFW_KEY_O: imOverlay_ = !imOverlay_; return true;
            case GLFW_KEY_M: imMainMenu_ = !imMainMenu_; return true;
            case GLFW_KEY_B: imBuffersWindow_= !imBuffersWindow_; return true;
            case GLFW_KEY_G: imProgramRecourceWindow_= !imProgramRecourceWindow_; return true;
            case GLFW_KEY_S: {
                if(mods == GLFW_MOD_CONTROL) {
                    imShaderWindow_ = !imShaderWindow_;
                    return true;
                } else {
                    return false;
                }
            }
        }

    }

}
