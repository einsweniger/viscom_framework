Introspection API
=================

While playing with shaders I've found myself switching a lot between C++ and GLSL.
Everytime there was a new uniform, I had to add it manually.

This back and forth proved to be quite tiresome, so I decided that I needed to have some automation.

Traditionally, the uniform storage location is queried via ``glGetUniformLocation`` with the program name (the GLuint returned from ``glCreateProgram``) and a known name of the uniform.
Since my goal was to detect and manipulate uniforms on runtime without touching c++ code, the approach was not feasible.

I was sure, that this problem was already solved by someone else, here are the approaches I came across.

What I've seen very often is plain string matching through the shader source code:

.. literalinclude:: ../src/glslViewer/shader.cpp
   :language: cpp
   :lineno-start: 44
   :lines: 44-75
   :emphasize-lines: 20,22,24,26-31
   :linenos:

Source: glslViewer_

.. _glslViewer: https://github.com/patriciogonzalezvivo/glslViewer/blob/23a8ddfc7e3a43e6128691d8e6a796dbe071c851/src/gl/shader.cpp#L44:L75

The only aspect differing throughout these approaches is the degree of sophistication in matching strings.
Some are checking if the `uniform` keyword is in front of the variable, others go as far as parsing the GLSL source to find uniforms.
Since this project was not about building a stable GLSL parser, I've tried finding other aproaches.

Large engines and frameworks build upon their own shading language to enable ``#include`` directives and dependency management.
Some other projects build their own programming language to interface tightly with GLSL (ex: scopes_)

.. _scopes: http://scopes.readthedocs.io/en/latest/module-glsl.html

All solutions have their merits, but none of them fit my project; so I started reading the OpenGL Specification.
I was delighted to find that OpenGL does support programmatic scanning for Uniforms; and even more!
Practically all useful information that is known by OpenGL through compiling and linking a shader program can be extracted.

Interfaces
----------

Starting from OpenGL core 4.6, chapter 7.3.1 ProgramInterfaces:

At the time of writing, there are 21 different interfaces; each supporting different properties.
The Uniform interface for example has 16 distinct properties; among these is the location of the uniform variable.

.. todo::
   when was introspection added?

Most, possibly all functions in this regard were encapsulated into the ``Basicinterface`` class.

Most functions in this class simply wrap calls to OpenGL, such that results are provided as a return value and not obtained via side effects.
Exceptions are:

.. doxygenclass:: minuseins::interfaces::BasicInterface
   :members: GetProgramResourceiv_vector, GetResource, GetNamedResource, GetAllResources, GetAllNamedResources

``GetProgramResourceiv_vector`` is necessary for two different interface properties: ``GL_ACTIVE_VARIABLES`` and ``GL_COMPATIBLE_SUBROUTINES``.
All methods that return `resources` provide structs for easier access: these contain the properties as key/value pairs and a name, if the interface provides one.

If necessary, a ``Basicinterface`` can be created in a custom manner, the following method predefines interfaces with their correct properties.

.. doxygenfunction:: minuseins::interfaces::get_interface


Resources
---------

The interfaces provide simple structs to convey information:

.. code-block:: cpp
   :linenos:

   using property_t = std::map<gl::GLenum, gl::GLint>;
   
   struct resource {
     resource(gl::GLuint resourceIndex, property_t properties);
   
     gl::GLuint resourceIndex;
     property_t properties;
   };
   
   struct named_resource : public resource {
     named_resource(std::string name, resource res);
     virtual void draw2D();
     virtual ~named_resource() = default;
     std::string name;
   };

They store their respective properties, the resource index and sometimes a name.
                     
Progression
-----------

I've tried to abstract and secure the Interface several times.
While it is possible to provide feedback on incorrect usage at compile-time, the overhead is not justifiable.
Using C++ templates, std::array with allowed enum-values and static_asserts proved cumbersome and difficult to maintain.
Especially if there was an oversight in mapping allowed properties to interfaces, the API users would have to correct the API, before they can continue.
The compilation times are already annoying, increasing them further does not seem necessary.
                     
Outcomes
--------

While working closely with the Specification, several shortcomings were discovered and reported to Khronos_.
The proposed changes_ were included in the OpenGL Core 4.6 (May 14, 2018).

.. _Khronos: https://github.com/KhronosGroup/OpenGL-API/issues/35

.. _changes: https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.withchanges.pdf#page=141


While researching, I've discovered that uniform blocks seem to be hard to use, since one has to implement them by closely following the format specified in the Core Spec.
Some solutions I've come across even use external tools to analyze the resulting struct definitions.
With this solution, uniform block creation might become easier.

The uniform interface provides the offset, type and size of the uniform and the uniform block interface allows to see binding points and the size of the uniform block at runtime.

Usage
-----

.. code-block:: cpp
   :linenos:
   :emphasize-lines: 3, 6, 20, 21

   using namespace minuseins::interfaces;  // get_interface
   void findSubroutines(GLuint program, std::vector<SubroutineUniform> uniforms) {
     auto interface = get_interface(GL_FRAGMENT_SUBROUTINE_UNIFORM, program);
   
     for (SubroutineUniform& uniform : uniforms) {
       auto compatibleSubroutines = interface.GetCompatibleSubroutines(uniform.resourceIndex, uniform.num_compatible_subroutines);
       // get active subroutine
       uniform.active_subroutine = GetUniformSubroutineuiv(uniform.location);
   
       for (auto& subIdx : compatibleSubroutines) {
         uniform.subroutines[subIdx] = subs.at(subIdx)->name;
         if (uniform.previous_active == subs.at(subIdx)->name) {
           uniform.active_subroutine = subIdx;
         }
       }
     }
   }

   void printUniformLocations(GLuint program) {
     auto interface = get_interface(GL_UNIFORM, program);
     auto res = interface.GetAllNamedResources();
     std::for_each(res.begin(), res.end(), [](auto& u){
       std::cout << u.name << ": " << u.properties.at(GL_LOCATION);
     })
   }

Caveats
-------

Usually, interfaces can be queried without regard to the state of the OpenGL context.

.. warning::

   Some corner cases (Subroutines for example) rely on the program being active.
   Otherwise some information might not be available.
