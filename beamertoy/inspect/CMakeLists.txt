add_library(inspect ProgramInspector.cpp
        #glwrap/types.cpp
        glwrap/interface.cpp
        glwrap/strings.cpp
        glwrap/resource.cpp

        models/resource.cpp
        models/interface.cpp
        #uniform/uniform_resource.cpp
        #uniform/generic_uniform.cpp
        #uniform/uniform_with_value.h
        #uniform/IntegerUniform.cpp
        #uniform/IntegerUniform.h
        )