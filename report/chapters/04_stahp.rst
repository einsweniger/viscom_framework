Summary
=======



Other Difficulties
------------------

Apart from fighting with the API, C++ provided quite some difficulties.
Abstracting the OpenGL program interface and storing differently typed container classes ate quite some time in this project.
There's almost 1900 lines of (working) discarded code concerning these two functions, not counting the changes.

C++ Templates still feel like unwieldy LISP programming when implementing more complex functions.
I've avoided them were possible as they add to the already long compile times.
The more complex templates were also discarded in favor of maintainability.

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

The beamer rig in the lab is awful to use.
I've written several powershell scripts to make synchronization and execution faster, but there's still room for improvment.
Windows' headstrong behaviour to "improve user experience" at the most inconvenient times stands quite contraty to what I'd expect from a stable development environment.
That's why I've decided to develop on Linux, knowing that some code will have to be ported for Windows later.

Final Thoughts
--------------

I'm pretty statisfied with the Introspection API and the underlying system for interacting with OpenGL.
I've put much work into it and hope that it will help others in understanding, teaching and experimenting with OpenGL and Shaders.
I might develop it further to create a standalone library with some examples.
Documentation is almost finished: it needs a little polishing, otherwise it's good to go.

The visuals that I've produced leave me unstatisfied.
I would have liked to make it more appealing and finish some of my ideas, but there was not enough time.
