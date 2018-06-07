Shadertoy import
================

.. figure:: /res/shadertoy.png

   several successfuly imported shadertoys and their rendering result.
            
To test the API, I've implemented rudamentary support for importing Shadertoys.
Shadertoy provides an API to access and download user content descriptions as JSON.
I've written two programs: one is in charge of retrieving shader descriptions and used assets, the other extracts headers and footers of the shaders.
They were written in python and javascript, respectively.
Headers and footers are necessary, since they are not included in the JSON data.

Importing through cereal was proved straight forward.
Using the Introspection API to hook into the uniforms provide state access to time and mouse position was trivial.
The same is also true for textures that are used in these programs.

Problems arise with multi-pass configurations, where one program's output is used by another.
Since the beamer API resource management is strictly file based, there is no possibility to manage generated textures.
Users can set the texture id manually, so it's only a small hurdle.

When the JSON description is imported, textures are automatically loaded from disk and attached to the program.
Included shader code is then written to the resources folder, with header and footer inserted as includes.
The resource manager is instructed to reload it from disk, as it cannot accept shader code as string.

Benefits
--------

In this application user can switch between rendering loaded programs and imported shadertoys.
After the shadertoys are imported their shader source code can also be modified and recompiled like the other shaders.

Most of these shaders were made configurable by their authors to highlight various aspects of the demonstrated technique.
These are either toggled through defines or ``const`` variables.
Replacing defines with uniforms or replacing ``const`` with ``uniform`` enables toggling these variations without recompiling.

