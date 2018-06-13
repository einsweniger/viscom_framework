Introspection API
=================

The implementation makes liberal use of unique pointers and abstract base classes to provide an extensible API.
The ``ProgramInspector`` detects active resources in an OpenGL Program and provides basic information about their state through ImGui.

Where desired, the default behavior can be customized to allow more interaction with these resources.
This is achieved by providing a handler for a specific interface, one example would be uniforms.
Besides uniforms, this includes subroutines, uniform blocks, program inputs/outputs and more.

The inspector will call attached handlers when a program is recompiled and provide basic information: the name of the resource and their properties as key/value pairs.
Handlers can substitute this ``named_resource`` by their own implementation, which must derive from this class.
``named_resource`` provides a ``virtual draw2d`` function that deriving classes sould override to allow interaction.

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

Within my application there are four builders:

- ``ExternalUniformBuilder`` is concerned with external input like time or camera position.
- ``TrackedUniformBuilder`` is the first fallback if the uniform does not match any rules. It will add uniforms with a float value or vector to the sync tracker.
- ``ShaderToySamplerBuilder`` will set parameters for samplers imported from Shadertoys, it will load textures and set texture wrapping parameters.
- ``UniformBuilder`` is the last fallback and provides Uniforms for interaction on the fly.

.. figure:: ../out_doxygen/all/structminuseins_1_1handlers_1_1UniformBuilder__inherit__graph.*

   Derived class hierarchy of the different builders, defines fallback strategy.

Other Handlers
--------------

Besides uniforms, the reference implementation provides three other main handlers: Program Output, Subroutine Uniforms and Uniform Blocks.

.. figure:: ../out_doxygen/all/structminuseins_1_1resource__handler__inherit__graph.*

   The abstract resource handler and its derivatives. ``SceneSubroutineHandler`` is application specific.


``ProgramOutputHandler`` is in charge of rendering variables declared as ``out vec4 variableName;``.
It assumes the variable to be color output from the fragment shader.
As allocating back buffers depends on the used framework it will call an additional callback function during ``postInit``.
This function should be in charge of creating these Textures.
If the program was rendering to a texture its result can be drawn through the GUI.
This allows to create additional outputs for visual debugging, like drawing normals or verifying shader inputs.

Uniform blocks were not decoupled from the beamer framework:
their use is to provide accessible usage of uniform blocks without knowledge of the layout specific details of the OpenGL standard.
I've been using them to bunch up uniforms visually, they also can be used to share information across shaders.

Subroutines allow to select from a set of functions defined in the shader.
This project uses them to select from:

- SDF scenes defined or included through the fragment shader,
- several different ray-marching implementations and
- postprocessing effects.

Other possible uses might be graphics settings (anti-aliasing methods) or comparing the output of different implementations.

.. figure:: /res/handlers.png

   The outputs of the implemented handlers.

Possible Improvements & Ideas
-----------------------------

Uniform blocks override the upload functions of uniforms belonging to the block.
With this implementation it's not possible to remove the override once the uniform moves out of the block.
Assing a ``preInit`` function to the inspector and handlers would allow to remove these overrides.

This architecture proves quite challenging to serialize uniforms through cereal.
All type information has to be reconstructed through dynamic casting the uniform back to it's derived class.
When a override function was set and is expected to be the same after deserialization, I'm still not quite sure how I'd accomplish that.

Together with ImGui's Drag and Drop functions, this API could be extended to create connections between program outputs, external data sources and uniforms.
It's just a rough idea: declaring resources with provided and accepted types so they can be connected:
e.g. a uniform float accepts a float value, ``currentTime`` provides a float value so the uniform resource can update it's value from there.
Another example would be to connect one program output (texture) to another program's sampler uniform.

I suspect that using this at larger scale might not be very performant, as these structures call back and forth between several data sources.
It would be interesting to provide some automation and generate code to implement better performing structures.
