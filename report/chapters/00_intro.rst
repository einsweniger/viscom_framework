Introduction
============

Goal: Build something that has potential for an entry with 64k intro on windows.
SubGoal: Draw pretty visuals
SubGoal: create music
SubGoal: sync visuals to music.
SubGoal: optimize resource usage in storage (64 KiB executable) and draw time (60 fps).
Goal: should should work with beamer setup in lab.
SubGoal: Test API and report problems or provide fixes.

The goal of this project is to create an audio-visual impression that might qualify as an entry in competition parties.
For a valid entry, several rules have to be met. The following rules were taken from Revision_:

- Maximum file size is 65536 bytes for the executable. All other files in the archive will be deleted before showing the entry in the competition.
- Maximum running time: 8 minutes (including loading/precalc).
- We will not install any additional Runtimes, SDKs, Codecs, Drivers etc on the compo machine. This means that, among others, msvcr70.dll, msvcr71.dll and msvcr80.dll will not be available.
- You may not use the contents of the Windows "Media" or "Music Samples" directories. These directories will be deleted on the compo machine.

.. _Revision: https://2018.revision-party.net/compos/pc

As stated, no resources other than the executable are allowed.
Putting this into perspective: the amount of music encoded as MP3 with 128kb/s would leave room for rougly 4s of audio.
Audio and Visuals must be generated during runtime either proceduraly or through space effcient instructions.
For bonus points visuals either respond to the music or are synced manually to match the rythm.

The other priority of this project is to run on the array of beamers for two reasons:
using the beamers to display something pretty and also to test the API exposed by the framework (to report and fix shortcomings).

To achieve small sizes, several techniques are used.
Object meshes are generated procedurally, some choose to do render without meshes.
Raymarching Signed Distance Functions can completely remove the need to use Meshes and instead define geometry through screen space shaders.
Since shaders are text, compressing them yields far better compression ratios than tesselated object meshes.

In general, audio is synthesized during runtime.
Most groups write their own synthesizers to maximize compression with their own music description format.

Achieved Goals
--------------

Due to several difficulties not very many of these goals were achieved:

Currently, the executable weighs in at 48 MiB, compressing it with UPX_ leads to rougly 13 MiB.
This does also not include the shaders and the music.

This project did neither produce a synthesizer, nor self made music.

The visuals are not as long as the chosen piece of music and, compared to entries in public competitions, not very pleasing.

At time of writing the project does not produce the same result on the beamers as it does runinng without.

Failing pretty much all goals set from the beginning, there's also some other outcomes.

.. _UPX: https://upx.github.io/

Outcomes
--------

**Introspection API**: to minimize recompiling the C++ code, an API was built that automatically detects uniforms in a program and allows manipulating their values.
Built upon this API, it's possible to **import Shadertoys**. This was done to help understanding some SDF and raymarching techniques.
**Audio playback** and a **Sync Tracker** were implemented to automate manipulation of uniforms and other application states (e.g. camera position).
These were all combined in a **GUI** to control all of the above at runtime.



