Outcomes and Reflexion
======================


beamer framework evaluation
===========================

Implementing this functionality also hightlighted several limitations of the beamer framework.

resource management
-------------------

While using strictly file based resource management is great for execution, it's a pain for development.
Especially since the shader compilation throws ``std::runtime_error`` instead of ``viscom::resource_loading_error``.
Catching runtime errors is discuraged throughout all platforms, so a typo in a shader include would normally terminate the program.
This entails losing all state that was not serialized to disk.

synchronization
---------------

sync input is a joke, don't call it sync when it does not do that


configuration management
------------------------

is a mess. Having them included in the cmake is neat, but:
I've been changing framework configs by hand and scripts.
Why? Because changing from "localms" to "single" means touching cmake configs.
Touching cmake files will recompile at least 5 g3log classes and then several files from the core and implementation.
Generate all of them at once, leave the user to choose.

The calibration data cannot be tested, it's not provided.

build process
-------------

is a fucking mess.
I've tried several times to restructure it.
No dependency management, changing a linker flag recompiles everything.
Well at least everything on the target.
The external libs that know how to use cmake won't. Sometimes.
I can no longer count how many times I've had to recompile glbinding because of this mess.

Sure. might not be as harsh with Visual Studio.
They take the CMake generated solutions as recommendations rather than instructions.

There's no clue how LOCAL_ONLY affects anything or when to set it.
I've had so many iterations of trial an error to find the correct permutation of all config flags. not fun.

I'd rather see one or several static libraries than a core depending on the implementation.
sgct should be included in the build process, in case Visual Studio decides to be incompatible to it's previous artifacts.

core structure
--------------

Tight coupling of core: recompile at 40-70 units when touching AppImpl.
More decoupling plz.

src_nosgct should be a dummy that looks like sgct, like with the tuio stub.
-> less ifdefs.

overall accessibility
---------------------

While the framework provides several nice abstractions, they are often more limiting than enabling.

The GPUprogram does not allow to access the shaders, so there is no access to the shader code to edit or display it within the app.
Deriving from GPUprogram is impossible, it's final.
The other method would be to copy it and modify it to my liking.
But, copying Resource based classes to modify them it in my own code cascades into a series of consequences as it's tightly coupled into the rest of the framework.
For example, FullScreenquad depends on GPUProgram, so I'd have to rewrite that.
The ApplicationNodeBase has a method to generate a FullScreenQuad, by forking FSQ in my code I'm unable to use that method.
I can no longer depend onto the ResourceManager since all implementing classes are hardcoded into it.
Same goes for the animation manager.

A FullscreenQuad does not allow mutable access to its program, this access can be obtained by using the resource manager.
This has been a an annoying process of trial an error on how the arguments of all of this work.
Same holds for texture descriptors.

While building parts that concern audio playback, I've been wanting to store the Fourier coefficients as texture.
There are two reasons for this: shaders can react to these values and to display the texture in ImGui.
The ``GLTexture`` abstraction does not allow access to the texture id, it's only accessible to some ``friend`` classes.

Regarding all the above points:
As I spend more time developing an application than running it, I'd rather see a change with this behavior.

So the easy way out was to reach into the core and change it.
