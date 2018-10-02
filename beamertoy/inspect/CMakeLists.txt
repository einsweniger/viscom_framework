add_library(inspect
        ProgramInspector.cpp
        glwrap/interface.cpp
        glwrap/resource.cpp

        models/resource.cpp
        models/interface.cpp
        models/uniform.cpp

        uniform/generic_uniform.cpp
        uniform/UniformTypes.cpp
        uniform/UniformHandler.cpp
        uniform/DefaultBuilder.cpp
        uniform/IntegerUniform.cpp
        uniform/FloatUniform.cpp
        uniform/BooleanUniform.cpp
        uniform/DoubleUniform.cpp
        uniform/UnsignedUniform.cpp
        uniform/SamplerUniform.cpp
        )
target_include_directories(inspect PRIVATE VISCOMCore INTERFACE cereal)
target_link_libraries(inspect PUBLIC VISCOMCore glbinding::glbinding glbinding::glbinding-aux)
set_property(TARGET inspect PROPERTY CXX_STANDARD 17)