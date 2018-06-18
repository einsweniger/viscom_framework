Fragment Shaders
================

Detail research and implementation for fragment shaders.

Signed Distance Functions
-------------------------

I've merged mercury's hg_sdf_ library (hg) and Inigo Quilez's Primitives_ (iq) on signed distance functions.
While most of their operations and functinos were similar they had some differences:
hg functions are built for speed, iq's focus is on quality and correctness.
Both are well documented, iq's articles_ provide additional information.

.. _hg_sdf: http://mercury.sexy/hg_sdf/
.. _articles: http://iquilezles.org/www/index.htm
.. _Primitives: https://www.shadertoy.com/view/Xds3zN

Rendering a "box" with hg_sdf works like this:

.. code-block:: glsl

   float sdfBox( vec3 p, vec3 b ){
     vec3 d = abs(p) - b;
     return precise_sdf(d);
   }

where ``p`` is the position, and ``b`` its bounds.
``sdfBox`` is one of the simpler SDF with only two parameters to remember.

I've thought about several ways to simplify shader and scene generation from a user's perspective.
One of them was to use structs to describe the scene and provide overrides for rendering:

.. code-block:: glsl

   struct Box {
       vec3 position;
       vec3 bounds;
   };
   float sdf(vec3 position, Box b) {
       return sdfBox(position-b.position, b.bounds);
   }

The reason was two-fold:

- structs have fully named and easy to remember members and could be rendered through an  overloaded ``sdf`` function.
- A C++ application could potentially generate the shader source from a scene description of primitives.
  Constructive Solid Geometry approaches could optimize the functions for run time.

This approach was not pursued further: defining the structs and corresponding overrides was labour-intensive.
Other variables (e.g. available OpenGL Extensions) for generating shaders from scene descriptions either reduce re-usability or add significant overhead to compensate.
Additionally, this approach would limit flexibility, so I've decided against it.

Ray Marching
------------

Shadertoy served as primary source for raymarching, SDF and associated techniques.
Mercury recommends implementing the "Enhanced Sphere Tracing" algorithm (2014, Keinert et. al.: EuroGraphics_).
The code of their paper proved straightforward to implement in GLSL.

.. _EuroGraphics: http://doi.org/10.2312/stag.20141233

iq implementation is simpler and is widely used throughout Shadertoy, but has its limitations:
It's designed for very small scenes and performs not quite as nice for long rays and high resolutions.
'Simple' Scenes work well withing their bounds and shortcuts: in general, the Enhanced algorithm is faster and produces nicer edges.

All ray marching algorithm performances degrade for large object counts: each object/functions adds to the calculation time for the rays.
While repetitions can be achieved with simple modulus calculation, they can no longer be rendered with distinctly different colors, positions or textures (Example_ does not render fast for high resolutions).

.. _Example: https://www.shadertoy.com/view/MdlBW7


Notable Techniques
------------------

Grid Traversal
^^^^^^^^^^^^^^

For scenes with many repeating objects and distinct colors are wanted, some implementations use multi-stage ray marching for repetition and identification of objects.
First, they separate their scene into a regular grid of cubes, cylinders or hexagons on a plane.
The ray-plane intersection is then used to identify the hit object's position and color.
Within the bounding box of the object, the ray is cast again within this limited range for an accurate rendering result.
An example can be seen in :numref:`cylgrid`

.. _cylgrid:
.. figure:: /res/cylgrid.png

   iq's Grid of Cylinders: https://www.shadertoy.com/view/4dSGW1

This technique requires the ray marching to be customized to the SDF primitives.
Since learning and understanding of this technique came rather late, my implementation does not have a purpose tailored ray marching algorithm.
If I were to implement it fully I'd select the ray marching algorithm through the fragment shaders subroutines, when the scene demands it.

SDF Fonts
^^^^^^^^^

The introduction features compound characters that rotate to reveal another word.
Inspiration was drawn from mattz's glyphspinner_ and the font sdf texture_ by Otavio Good.

.. _glyphspinner: https://www.shadertoy.com/view/XsVcDy
.. _texture: https://www.shadertoy.com/view/llcXRl

The glyphspinner uses expensive calculations to smooth the font surface and normals.
These had to be removed in order to achieve stable 60fps.
I had considered creating my own SDF font texture or bezier curves using `MSDF Gen`_, but could not make time for it.

.. _`MSDF Gen`: https://github.com/Chlumsky/msdfgen

SDF Visualization
^^^^^^^^^^^^^^^^^

To debug SDF compositions, I've modified cupe's `distance meter`_ to work with arbitrary visualization planes and camera positions.
As it also looks quite nice by itself, I used it as a canvas to draw on with distance functions.

.. _`distance meter`: https://www.shadertoy.com/view/ldK3zD


Other Sources
^^^^^^^^^^^^^

Further research material includes demo breakdowns of logicoma member ferris_.
He explains their architecture and techniques for audio and visuals during live streams.
Shadertoy user Shane_ writes beautiful and fast shaders with excellent documentation and explanations.

.. _ferris: https://www.youtube.com/channel/UC4mpLlHn0FOekNg05yCnkzQ
.. _Shane: https://www.shadertoy.com/user/Shane

