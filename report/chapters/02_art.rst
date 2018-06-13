Artistic Decisions
==================

Music
-----

For the music I've chosen "Together" by Mefjus_.
It features several rhythmic changes and includes both exciting and calm parts.

.. _Mefjus: https://soundcloud.com/mefjus/together

Music playback was implemented with the bass_ library.
It features simple playback controls, resource loading and access to FFT coefficients to measure audio intensity.

.. _bass: https://www.un4seen.com/

Synchronization of visuals to music was enabled by the tracker:

.. figure:: /res/tracker2s.png

   Tracker displays FFT data to the left.
   The current position (row) is displayed by magenta text color (0687).
   The color of the variable denotes the interpolation variant (linear, ramp, smooth, step)

Once connected, active uniforms request interpolated values from the tracker and upload them to the program.

Visuals
-------

.. figure:: /res/vis_small.png

   Several scenes from the application.
   
Visuals are produced entirely by fragment shaders.
There are two main shaders: the first produces the scene, the next applies screen-space processing effects.
The scenes and post processing effects are selected through subroutines at runtime.
Both can respond to the music's FFT output.

The scenes are rendered entirely through ray marching SDFs.
Scenes work like plugins, can be included by the main program and are provided as subroutines after recompilation.

Regrettably, there's not much to say about the visuals.
I've run into several time consuming problems during the implementation and could't focus on them as much as I'd liked.

The visuals run in sync to the music, scenes and postprocessing are selected to fit the audio.
There's a rudamentary PBR shader but no material system to fully exploit its capabilities.
