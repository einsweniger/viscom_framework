add_library(inspect
        ProgramInspector.cpp
        glwrap/interface.cpp
        glwrap/resource.cpp

        models/resource.cpp
        models/interface.cpp
        models/uniform.cpp
        models/program_output.cpp

        uniform/handler.cpp
        uniform/DefaultBuilder.cpp

        uniform/generic_uniform.cpp
        uniform/empty_uniform.cpp
        uniform/integer.cpp
        uniform/float.cpp
        uniform/boolean.cpp
        uniform/double.cpp
        uniform/unsigned.cpp
        uniform/sampler.cpp
        )
target_include_directories(inspect PRIVATE VISCOMCore INTERFACE cereal)
target_link_libraries(inspect PUBLIC VISCOMCore glbinding::glbinding glbinding::glbinding-aux)
set_property(TARGET inspect PROPERTY CXX_STANDARD 17)