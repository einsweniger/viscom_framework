Beamer Framework Evaluation
===========================

Implementing this functionality also hightlighted several limitations of the beamer framework.

Resource Management
-------------------

While using strictly file based resource management is great for execution, it's a pain for development.
Especially since the shader compilation throws ``std::runtime_error`` instead of ``viscom::resource_loading_error``.
Catching run time errors is discouraged throughout all platforms, so a typo in a shader include would normally terminate the program.
This entails losing all state that was not serialized to disk.

Configuration Management
------------------------

The framework configuration files are configured through Cmake.
While it's comfortable to have them generated it it's difficult to develop with.
That's why I've been changing framework configs by hand and scripts.
Why? Because changing from "localms" to "single" means touching Cmake configs.
Touching Cmake files will needlessly recompile at least five g3log classes and then several files from the core and implementation.

It was rather painful to pin down a problem with the configuration for SGCT.
All provided configurations work, I've even written a python script that generates configurations to test against.
Still, when running the application on the beamer array, the looking direction was skewed.

As the calibration data is not provided the real thing cannot be tested locally.
Even if it were, I doubt it would work for testing.
Some dummy data would be nice, and it would also be neat to document that the ``clusterR.xml`` does not describe the actual configuration.
That would help to prevent future confusion.

There's no clue how ``LOCAL_ONLY`` affects anything or when to set it.
I've had so many iterations of trial and error to find the correct permutation of all config flags.

Build Process
-------------

Building the framework takes a long time which slows development iterations significantly.
I've tried several times to restructure the CMake configuration, but the dependencies inside the project preveted me from doing so.
It might be fine to recursively include the source folders, but not in such a large framework.
Without any dependency management changes in CMake configuration entail either a complete recompilation of the targer or recompilation of everything.
Changing a linker flag should not recompile all units.

The globbing also leads to the problem that, once a new compilation unit is added cmake has to glob again and many classes are unnecessarily recompiled.

To fix this, I can see two ways: either fix the CMake configuration or provide the core as a static library.
Since the core depends on the implementing class, a static library would need some minor changes, but I can't see that not working.
Fixing CMake would be very time consuming: to communicate dependencies to CMake, the dependencies of the code have to be figured out first.
.. SGCT should be included in the build process, in case Visual Studio decides to be incompatible to its previous artifacts.

Core Structure
--------------

The tight coupling of the core and the many dependencies on ``ApplicationNodeImplementation`` prevent fast iterations.
Some small change leads to recompilation of many core classes as they all have some dependency involving it.

I'm also not sure how the structure of the framework came to be.
Some of the differences between *src_sgct* and *src_nosgct* could possibly go away when using the *light* variant of sgct.

Overall Accessibility
---------------------

While the framework provides several OpenGL abstractions, they are often more limiting than enabling.

The GPUprogram does not allow to access the shaders, so there is no access to the shader code to edit or display it within the app.
Deriving from GPUprogram is impossible, it's final.
The other method would be to copy it and modify it to my liking.
But, copying Resource based classes to modify them it in my own code cascades into a series of consequences as it's tightly coupled into the rest of the framework.
For example, FullScreenquad depends on GPUProgram, so I'd have to rewrite that.
The ApplicationNodeBase has a method to generate a FullScreenQuad, by forking FSQ in my code I'm unable to use that method.
I can no longer depend on the ResourceManagers handed out by the ApplicationNode since all implementing classes are hard coded into it.
Same goes for the animation manager.
I'd like to see some decoupling here, so it enables drop in replacements where needed.

A FullscreenQuad does not allow mutable access to its program, this access can be obtained by using the resource manager.
This has been an annoying process of trial an error on how the arguments of all of this work.
Same holds for texture descriptors.

While building parts that concern audio playback, I've been wanting to store the Fourier coefficients as texture.
There are two reasons for this: shaders can react to these values and to display the texture in ImGui.
The ``GLTexture`` abstraction does not allow access to the texture id, it's only accessible to some ``friend`` classes.

Regarding all the above points:
As I spend more time developing an application than running it, I'd rather see a change with this behavior.
I'd suggest moving OpenGL abstractions to globjects because they provide more flexibility and are actively maintained.

Most of the time the easy way out was to reach into the core and change it.
After I was done with the project, I've removed some functions of the application in favor of leaving the core untouched.
The removed concern no mission critical functions, but they were rather nice to have for development.

Other Difficulties
==================

Apart from fighting with the API, C++ provided quite some difficulties.
Abstracting the OpenGL program interface and storing differently typed container classes ate quite some time in this project.
There's almost 1900 lines of (working) discarded code concerning these two functions, not counting the changes.

The incompatibilities between the Microsoft Compiler and the compilers available on Linux further complicated the matter.
While Clang (most of the time) provides better error reporting they are no longer compatilbe with GCC's STL.
Clang's implementation (6.0.0) does not support ``map::extract`` and their libc++ proved to be not straightforward to compile with.
Even now linking the executable fails while using clang.

Then there is the Nvidia linker for OpenGL SL shaders.
It was crashing so many times with invalid memory access and left me with no hints to the actual problem.
I suspect my usage of subroutines to be the root cause, from what I gathered it's not a widely used feature.

The OpenGL Core Specification had me running around in circles several times.
There were some partially undocumented variables and vaguely phrased behaviors that I've proposed fixes for.
My requested fixes were included in the most recent version of the Core Specification.
