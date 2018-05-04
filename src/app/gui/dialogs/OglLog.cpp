//
// Created by bone on 01.05.18.
//

#include <glbinding/glbinding.h>
#include <sstream>
#include <glbinding-aux/Meta.h>
#include <g3log/loglevels.hpp>
#include <g3log/g3log.hpp>
#include <glbinding/gl/gl.h>
#include "OglLog.h"
namespace minuseins::gui {

    void OglLog::callback(const glbinding::FunctionCall &call) {
        const auto error = gl::glGetError();
        if (gl::GL_NO_ERROR != error) {
            visible = true;
            std::stringstream callOut;
            callOut << "Error: " << glbinding::aux::Meta::getString(error) << "\n";
            callOut << call.function->name() << "(";
            for (unsigned i = 0; i < call.parameters.size(); ++i) {
                // callOut << call.parameters[i]->asString();
                callOut << call.parameters[i].get();
                if (i < call.parameters.size() - 1)
                    callOut << ", ";
            }
            callOut << ")";

            if (call.returnValue)
                callOut << " -> " << call.returnValue.get();
            AddLog("%s",callOut.str().c_str());
        }
    }

    void OglLog::Draw(const char *title, bool *p_open) {
        if(*p_open && visible)
            ShaderLog::Draw(title, p_open);
    }
}
