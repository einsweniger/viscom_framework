Case Study: Shadertoy
=====================

During my research on ray marching and signed distance functions, I've been drawing heavily from Shadertoy_, both for explanations and inspiration.
Multiple browser tabs with opened shadertoys consistently crash the WebGL modules of my browsers, so this was not exactly fun.
With the discussed API already in place, it's fun and (mostly) easy to import JSON from the shadertoy API_.

.. _Shadertoy: https://www.shadertoy.com/
.. _API: https://www.shadertoy.com/api

Used tools
----------

Cereal provides a very convenient way of (de)serialization from and to various formats.
But, before cereal is useful, you'll need structs to deserialize to.

quicktype
^^^^^^^^^

Figuring out the structs from reading JSON was error prone, so I dumped all shaders available through the shadertoy API.
This includes JSON descriptions of the Shaders, how they are connected and the available assets (textures, audio, video, ...).

After concatenation, I've dumped the array of retrieved shaders and had quicktype_ figure it out for me.
Quicktype offers a neat service of parsing JSON and providing structures for several languages, to give an excerpt of the result:

.. _quicktype: https://app.quicktype.io/


.. code-block:: cpp
   :linenos:

   enum class Username { CXW, DEMOFOX, LARA, OLLJ, PANIQ };

   struct Info {
       std::string id;
       std::string date;
       int64_t viewed;
       std::string name;
       Username username;
       std::string description;
       int64_t likes;
       int64_t published;
       int64_t flags;
       std::vector<std::string> tags;
       int64_t hasliked;
   };
   
   enum class Description { EMPTY };
   
   enum class Ctype { BUFFER, CUBEMAP, KEYBOARD, TEXTURE };
   
   enum class Filter { LINEAR, MIPMAP, NEAREST };
   
   enum class Internal { BYTE };
   
   enum class Srgb { FALSE };
   
   enum class Vflip { FALSE, TRUE };
   
   enum class Wrap { CLAMP, REPEAT };
   
   struct Sampler {
       Filter filter;
       Wrap wrap;
       Vflip vflip;
       Srgb srgb;
       Internal internal;
   };
   
   struct Input {
       int64_t id;
       std::string src;
       Ctype ctype;
       int64_t channel;
       struct Sampler sampler;
       int64_t published;
   };

The option to detect enums is neat, but problematic for serialization with cereal for several reasons.

The enums can be incomplete, which will lead to deserialization errors:
``Srgb`` is obvoiusly a boolean, ``Ctype`` is incomplete and even if it were complete it might change.
``Username`` is completely useless.

So instead of Enums, I've opted for the simpler method to just use std::string.
That also helps with serialization on cereal, you'd have to provide a conversion enum to string for every case, which seemed unnecessary in my use case.

Some ``int64_t`` were also reduced to smaller signed integers for compatibility with OpenGL interfaces.


Scripts
^^^^^^^

For retreiving the shadertoy content, a small python script was crated.
As toy descriptions do not include header (uniform declarations) and footer (main method) of the shader, it was necessary to find them.
To stay true to the original, a small Javascript script retrieves header and footer contents and writes them to disk.

External Uniform Values
-----------------------

To implement the uniforms used by shadertoy, another builder was created.
Most of them depend on values provided by sources beyond the control of the shader (like time, texture sizes or mouse position).
``ExternalUniformBuilder`` derives from ``UniformBuilder`` and tries to match on known uniform names, this is just an excerpt:

.. code-block:: cpp
   :linenos:

   std::unique_ptr<generic_uniform> ExternalUniformBuilder::operator()(
       named_resource res) {
     if (res.name == "iDate" && res.properties.at(gl::GL_TYPE) == gl::GL_INT) {
       return std::make_unique<detail::iFrame>(std::move(res));
     }
     if (res.name == "iTime" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
       return std::make_unique<detail::iTime>(std::move(res), appImpl);
     }
     if (res.name == "iMouse" &&
         res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
       return std::make_unique<detail::iMouse>(std::move(res));
     }
     std::cout << "warning, uncaught " << res.name << std::endl;
     return UniformBuilder::operator()(std::move(res));
   }

When uniform name and type match, a purpose-built class is returned; otherwise we delegate to the base class.
The purpose-built classes are very straight forward:

.. code-block:: cpp
   :linenos:

   struct iTime : FloatUniform {
     ApplicationNodeImplementation *appImpl;
   
     iTime(const named_resource &arg, ApplicationNodeImplementation *appImpl)
         : FloatUniform(arg), appImpl(appImpl) {}
   
     bool get_updated_value() override {
       if (do_value_update) {
         value[0] = appImpl->currentTime_;
       }
       return true;
     }
   };
   
   struct iDate : FloatUniform {
     using FloatUniform::FloatUniform;
   
     bool get_updated_value() override {
       if (do_value_update) {
         std::time_t time_ = std::time(nullptr);
         auto tm = std::localtime(&time_);
         value[0] = tm->tm_year;
         value[1] = tm->tm_mon;
         value[2] = tm->tm_mday;
         value[3] = tm->tm_hour * 3600.0f + tm->tm_min * 60.0f + tm->tm_sec;
       }
       return true;
     }
   };
   
   struct iMouse : FloatUniform {
     using FloatUniform::FloatUniform;
   
     bool get_updated_value() override {
       // left, right, middle + extras.
       // TODO value[2], value[3] ?
       if (do_value_update) {
         if (ImGui::GetIO().MouseDown[1]) {
           value[0] = ImGui::GetIO().MousePos.x;
           value[1] = ImGui::GetIO().MousePos.y;
         } else {
           value[0] = 0;
           value[1] = 0;
         }
       }
       return true;
     }
   };

They all override their value update function, and that's it.
``iTime`` needs access to the time reported by our framework, so it's also given a pointer to find it.
This enables modifying time arbitrarily through an ImGui::DragFloat.


Texture Samplers
----------------

When the input is a texture, the implementation is straight forwart.
Given the location of a texture, our framework provides a mechanism for loading at runtime.
Textures were stored such that the mechanism can find them when handed the source from JSON.
Uploads are similar to ``SamperUniform``, with the exception that some parameters are initialized from the values provided in JSON.

There are two problems, that are not solved yet:

Program Output as Input
^^^^^^^^^^^^^^^^^^^^^^^

There's no mechanism to find another programs outputs.
If it's necessary to use output of another program (available as texture), there is currently no way wo find it.

Currently, the only option is to specify the texture id by hand.
Once the other program recompiles, the texture id might change and you'll have to adjust it manually.
All (stable) resource management in our framework is file-based, so no luck there.

Shadertoy Double Buffers
^^^^^^^^^^^^^^^^^^^^^^^^

Shadertoy allows to select various input sources for the iChannel[0-3] samplers.
As mentioned, these also include outputs of other programs.
It's also allowed to create feedback loops, feeding output back into the sampler in the next pass.

As OpenGL does not allow to read from and write into the same texture, Shadertoy solved it by allocating two buffers.
Like in double buffering techniques, their roles switch between input and output for each renderpass.
This was not implemented, leaving some shaders that depend on this feature defunct.

One possible option would be to switch from texture based samplers to images, which would entail rewriting the source of shadertoys.

This should be easier to solve than managing outputs, but was really out of the scope of my project.
