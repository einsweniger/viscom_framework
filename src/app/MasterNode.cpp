/**
 * @file   MasterNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the master application node.
 */

#include "MasterNode.h"
#include <imgui.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void checkGlError() {
    GLenum err;
    while (true) {
        err = glGetError();
        switch (err) {
            case GL_INVALID_ENUM:
                printf("GLerror: invalid enum\n");
                break;
            case GL_INVALID_VALUE:
                printf("GLerror: invalid value\n");
                break;
            case GL_INVALID_OPERATION:
                printf("GLerror: invalid operation\n");
                break;
            case GL_STACK_OVERFLOW:
                printf("GLerror: stack overflow\n");
                break;
            case GL_STACK_UNDERFLOW:
                printf("GLerror: stack underflow\n");
                break;
            case GL_OUT_OF_MEMORY:
                printf("GLerror: out of memory\n");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                printf("GLerror: invalid framebuffer operation\n");
                break;
            case GL_CONTEXT_LOST:
                printf("GLerror: context lost\n");
                break;
            case GL_TABLE_TOO_LARGE:
                printf("GLerror: table too large\n");
                break;
            case GL_NO_ERROR:
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
    void drawProgramWindow(GLuint id, bool* p_open)
    {
        const std::unordered_map<GLint, std::string> interfaces {
            {GL_UNIFORM, "GL_UNIFORM"},
            {GL_UNIFORM_BLOCK, "GL_UNIFORM_BLOCK"},
            {GL_PROGRAM_INPUT, "GL_PROGRAM_INPUT"},
            {GL_PROGRAM_OUTPUT, "GL_PROGRAM_OUTPUT"},
            {GL_BUFFER_VARIABLE, "GL_BUFFER_VARIABLE"},
//            {GL_ATOMIC_COUNTER_BUFFER, "GL_ATOMIC_COUNTER_BUFFER"},
//            {GL_SHADER_STORAGE_BLOCK, "GL_SHADER_STORAGE_BLOCK"}
        };
        const std::unordered_map<GLint, std::string> interfaceProperties {
            {GL_ACTIVE_RESOURCES, "GL_ACTIVE_RESOURCES"},
            {GL_MAX_NAME_LENGTH, "GL_MAX_NAME_LENGTH"},  // not for ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER
//            {GL_MAX_NUM_ACTIVE_VARIABLES, "GL_MAX_NUM_ACTIVE_VARIABLES"},  // only for ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK
//            {GL_MAX_NUM_COMPATIBLE_SUBROUTINES, "GL_MAX_NUM_COMPATIBLE_SUBROUTINES"}  // only for {VERTEX,GEOMETRY,FRAGMENT,COMPUTE}_SUBROUTINE_UNIFORM , TESS_{CONTROL,EVALUATION}_SUBROUTINE_UNIFORM
        };
        const std::unordered_map<GLint, std::vector<GLint>> resourceProperties{
            {GL_UNIFORM,         {GL_NAME_LENGTH, GL_ARRAY_SIZE}},  // ARRAY_STRIDE , BLOCK_INDEX , IS_ROW_MAJOR , MATRIX_STRIDE, ATOMIC_COUNTER_BUFFER_INDEX
            {GL_UNIFORM_BLOCK,   {GL_NAME_LENGTH, GL_ACTIVE_VARIABLES, GL_BUFFER_BINDING, GL_NUM_ACTIVE_VARIABLES}},  // BUFFER_DATA_SIZE
            {GL_PROGRAM_INPUT,   {GL_NAME_LENGTH, GL_ARRAY_SIZE, GL_IS_PER_PATCH, GL_LOCATION, GL_LOCATION_COMPONENT}},
            {GL_PROGRAM_OUTPUT,  {GL_NAME_LENGTH, GL_ARRAY_SIZE, GL_IS_PER_PATCH, GL_LOCATION, GL_LOCATION_COMPONENT, GL_LOCATION_INDEX}},
            {GL_BUFFER_VARIABLE, {GL_NAME_LENGTH, GL_ARRAY_SIZE}},  // ARRAY_STRIDE , BLOCK_INDEX , IS_ROW_MAJOR , MATRIX_STRIDE
        };
        if(ImGui::Begin("GPUProgram", p_open)) {
            GLint activeProg;
            glGetIntegerv(GL_CURRENT_PROGRAM, &activeProg);
            ImGui::BulletText("Active Program: %d (%d)", activeProg, id);
            for(auto interface : interfaces) {
                if(ImGui::TreeNode(interface.second.c_str())){
                    for (auto property : interfaceProperties) {
                        GLint value;
                        glGetProgramInterfaceiv(id, interface.first, property.first, &value);
                        ImGui::Text("%s: %d", property.second.c_str(), value);
                        value = 0;  // you have to reset the value by yourself, because GLFW won't.
                    }
                    ImGui::TreePop();
                }
            }
            GLint maxSubRoutines,maxSubroutineUniformLocations,activeSubUniforms;
            glGetIntegerv(GL_MAX_SUBROUTINES, &maxSubRoutines);
            glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubroutineUniformLocations);
            ImGui::BulletText("GL_MAX_SUBROUTINES: %d", maxSubRoutines);
            ImGui::BulletText("GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS: %d", maxSubroutineUniformLocations);
            if(ImGui::TreeNode("Subroutine details")) {
                glGetProgramStageiv(id, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &activeSubUniforms);
                ImGui::Text("Active Subroutines: %d", activeSubUniforms);
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
            if(imProgramRecourceWindow_) drawProgramWindow(GetActiveGPUProgram()->getProgramId(), &imProgramRecourceWindow_);
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
