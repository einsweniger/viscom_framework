Specializing Interfaces
=======================

If more control is wanted or necessary, the ``Programinspector`` allows to add handlers for an interface.
Handlers are supposed to implement the virtual abstract base class ``resource_handler``, containing 3 functions.

.. code-block:: cpp
   :linenos:

   using res_ptr = std::unique_ptr<named_resource>;
   using container = std::vector<res_ptr>;
   
   struct resource_handler {
     virtual res_ptr initialize(ProgramInspector& inspect, named_resource res) = 0;
   
     virtual void postInit(ProgramInspector& inspect, container& resources) = 0;
   
     virtual void prepareDraw(ProgramInspector& inspect, container& resources) = 0;
   };

After complete implementation, resource handlers can be added to the ProgramInspector:

.. code-block:: cpp
   :linenos:

   auto gpi_ = std::make_unique<ProgramInspector>(progId, shaderName);
   gpi_->setHandler(gl::GL_PROGRAM_OUTPUT, std::make_unique<ProgramOutputHandler>());
   // possibly add more handlers for other interfaces.
   gpi_->initialize(); //will call the handler's initialize and postInit functions.

To provide an example, we're using the SubroutineUniformHandler.

Initialize
----------

.. doxygenfunction:: minuseins::resource_handler::initialize

.. code-block:: cpp
   :linenos:

   std::unique_ptr<named_resource> SubroutineUniformHandler::initialize(
       ProgramInspector& inspect, named_resource res) {
     auto sub = std::make_unique<SubroutineUniform>(stage, std::move(res));
     try {
       auto residx = inspect.GetResourceIndex(subroutineUniformEnum(stage), sub->name);
       auto& old_res = inspect.GetContainer(subroutineUniformEnum(stage)).at(residx);
       auto& old_uniform = dynamic_cast<SubroutineUniform&>(*old_res);
       sub->previous_active = old_uniform.subroutines.at(old_uniform.active_subroutine);
     } catch (std::out_of_range&) {
     }
     return std::move(sub);
   }

During initlization a new ``SubroutineUniform`` is created from the information provided through ``res``.
It would be just fine to return the created resource, but we try to restore the previous state of the variable.

During ``initialize`` The ProgramInspector reference allows to retreive previous values of the resources for the handled interface.
We use ``GetResourceIndex`` to find the previous SubroutineUniform index by name; which in turn is used to get the old resource.
Since the ProgramInspector stores unique_ptrs of named_resouce; you might need to cast it back to it's original type.
When everything succeeded, we store the previous value.

.. note::

   Dynamic casting to a reference of this object will ensure it's type is correct and throw an exception.
   Casting to anything else might result in undefined behavior.

.. warning::

   Handlers' initialize methods are called in no particular order; don't depend on it.
   This might become a problem if you'd like to access old values of other interfaces.
   

After Initialization
--------------------

After all interfaces were succesfully initialized in the first step, ``postInit`` is called.

.. doxygenfunction:: minuseins::resource_handler::postInit

It will provide you with the proviously initialized resources, and the reference to the PI.

.. code-block:: cpp
   :linenos:

   void SubroutineUniformHandler::postInit(ProgramInspector& inspect,
                                        named_resource_container& resources) {
     auto& subs = inspect.GetContainer(subroutineEnum(stage));
     auto interface = inspect.GetInterface(subroutineUniformEnum(stage));
   
     for (auto& res : resources) {
       auto& uniform = dynamic_cast<SubroutineUniform&>(*res);
       auto compatibleSubroutines = interface.GetCompatibleSubroutines(
           uniform.resourceIndex, uniform.num_compatible_subroutines);
       uniform.active_subroutine = GetUniformSubroutineuiv(uniform.location);
   
       for (auto& subIdx : compatibleSubroutines) {
         uniform.subroutines[subIdx] = subs.at(subIdx)->name;
         if (uniform.previous_active == subs.at(subIdx)->name) {
           uniform.active_subroutine = subIdx;
         }
       }
     }
   }

Since it is now safe to access resources of other interfaces, we can retreive through the subroutine interface (e.g. ``GL_FRAGMENT_SUBROUTINE``).
We check which subroutines are actually compatible with the respective uniform and restore the previous value where applicable.

.. note::

   When there was no handler attached to the subroutine interface, the default behaviour is to store unique_ptrs to named_resources.


Preparing the Draw Call
-----------------------

If necessary you might need to communicate some information about the resources to OpenGL.

.. doxygenfunction:: minuseins::resource_handler::prepareDraw

Sometimes, you can delegate this directly to the resource; other times you might need access to all resources at once.
SubroutineUniforms are, again, the best example:

.. code-block:: cpp
   :linenos:

   void SubroutineUniformHandler::prepareDraw(
    ProgramInspector& inspect, named_resource_container& resources) {
     if (resources.size() == 0) return;
   
     auto active_subs = std::vector<gl::GLuint>(resources.size());
     for (auto& res : resources) {
       auto& uniform = dynamic_cast<SubroutineUniform&>(*res);
       uniform.get_update();
       active_subs.at(uniform.location) = uniform.active_subroutine;
     }
     gl::glUniformSubroutinesuiv(
         stage, static_cast<gl::GLsizei>(active_subs.size()), &active_subs[0]);
   }

All subroutine uniform values must be set at once through an array.
Here, all active subroutines are updated, their values collected in a vector and finally set by the last call.

You might be wondering how the resources do their job and how they'll receive state updates.
This will be discussed in the next chapter.

Caveats
-------

Problem: As mentioned, you should not depend on values of other interfaces during ``initialize``.
Their content might already have changed (if you depend on an old value) or their ``initialize`` might be called after your handler.

Solution: Where necessary you might add another abstract method (preInit) to the ``resource_handler`` and call these in the ProgramInspector before initialization.

I've thought a long time about locking the inspector down: handing out only const references through ``postInit`` and ``initialize`` might be nicer.
While it might prevent you from shooting yourself in the foot, this would also limit flexibility; so I decided against it.
