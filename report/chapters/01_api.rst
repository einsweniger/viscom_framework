Introspection API
=================

The implementation makes liberal use of unique pointers and abstract base classes to provide an extensible API.
The ProgramInspector detects resources in an OpenGL Program and provides basic information about their state through ImGui.

Where desired, the default behavior can be customized to allow more interaction with these resources.
This is achieved by providing a handler for a specific interface, one example would be uniforms.
Besides uniforms, this includes subroutines, uniform blocks, program inputs/outputs and more.

The inspector will call attached handlers when a program is recompiled and provide basic information: the name of the resource and their properties as key/value pairs.
Handlers can substitute this ``named_resource`` by their own implementation, which must derive from this class.

Uniforms
--------

.. figure:: /out_doxygen/all/structminuseins_1_1handlers_1_1generic__uniform__inherit__graph.*

   Dependency graph for ``generic_uniform`` (builds upon ``named_resource`` and provides the most generic interface for uniforms).
   
The lower half of these specializations is concerned with storing and manipulating uniform values through user interaction.
Implementations on the far right build upon these to provide automation:
If the program contains a ``uniform float iTime;`` the uniform will retrieve its value from the current time.
These specializations also allow disabling the external source of data so the user can manipulate themselves.

When no explicit uniform implementation is provided, the handler will use more generic implementations as fallback.
If the user was to define a new uniform (say ``uniform vec3 material;``) the basic implementation is used (``FloatUniform``) so the values can be adjusted by the user.
Defined, but unused uniforms will not be available through this interface.

Each class deriving from ``UniformWithValueVector<T>`` provide their own upload and draw function:
a ``float vec3`` is drawn as ``ImGui::ColorEdit3`` and uses ``gl::glUniform3fv`` to send its value to OpenGL.
When a boolean uniform is encountered ``ImGui::Checkbox`` and ``gl::glUniform1iv`` are used for drawing and sending.

Other Handlers
--------------

Besides uniforms, the reference implementation provides three other handlers: Program Output, Subroutine Uniforms and Uniform Blocks.

If the program was rendering to a texture it's result can be drawn through the GUI.
This allows to create additional outputs for visual debugging, like drawing normals or verifying shader inputs.

Uniform blocks were not decoupled from the beamer framework:
they provide accessible use uniform blocks without knowledge of the layout specific details of the OpenGL standard.

Subroutines allow to select from a set of functions defined in the shader.
This project uses them to select from:

- SDF scenes defined or included through the fragment shader,
- several different ray-marching implementations and
- postprocessing effects.

Other possible uses might be graphics settings (anti-aliasing methods) or comparing the output of different implementations.

.. figure:: /res/handlers.png

   The outputs of the implemented handlers.


