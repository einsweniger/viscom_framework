add_library(inspect
        ProgramInspector.cpp
        #glwrap/types.cpp
        glwrap/interface.cpp
        glwrap/resource.cpp

        models/resource.cpp
        models/interface.cpp
        uniform/UniformTypes.cpp
        uniform/UniformHandler.cpp
        uniform/DefaultBuilder.cpp
        #program_output/ProgramOutputHandler.cpp
        #builders/ShaderToySamplerBuilder.cpp
        #builders/ExternalUniformBuilder.cpp
        #uniform/uniform_resource.cpp
        #uniform/generic_uniform.cpp
        #uniform/uniform_with_value.h
        #uniform/IntegerUniform.cpp
        #uniform/IntegerUniform.h
        )
target_include_directories(inspect PRIVATE VISCOMCore INTERFACE cereal)
target_link_libraries(inspect PUBLIC VISCOMCore glbinding::glbinding glbinding::glbinding-aux)
set_property(TARGET inspect PROPERTY CXX_STANDARD 17)