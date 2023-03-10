.. _Doc_BoxAlgorithm_StimulationListener:

Stimulation listener
====================

.. container:: attribution

   :Author:
      Yann Renard
   :Company:
      INRIA/IRISA

.. image:: images/Doc_BoxAlgorithm_StimulationListener.png

Prints each received stimulationto the log using the log level specified in the box config.

The stimulation listener is a debugging purpose box, with
the same idea as the :ref:`Doc_BoxAlgorithm_EBMLStreamSpy` box
but dedicated to stimulation streams. The idea is to dramatically
reduce the log verbosity so the output may be followed realtime
if necessary.

Inputs
------

.. csv-table::
   :header: "Input Name", "Stream Type"

   "Stimulation stream 1", "Stimulations"

This box can receive as many input as necessary. All the inputs
will be of type :ref:`Doc_Streams_Stimulation` in order to
be parsed by this the reader.

Stimulation stream 1
~~~~~~~~~~~~~~~~~~~~

This is the default input of this box.

.. _Doc_BoxAlgorithm_StimulationListener_Settings:

Settings
--------

.. csv-table::
   :header: "Setting Name", "Type", "Default Value"

   "Log level to use", "Log level", "Information"

Log level to use
~~~~~~~~~~~~~~~~

This setting indicates what log level will be used to
print the the received stimulations.

.. _Doc_BoxAlgorithm_StimulationListener_Examples:

Examples
--------

As an example, we could connect a clock stimulator to a
stimulation listener. Leave the default settings of the
clock stimulator, so it sends an \e OVTK_StimulationId_Label_00
stimulation every second. Now chose an appropriate log level
for the stimulation listener and press 'start'. If you're
familiar to what :ref:`Doc_BoxAlgorithm_EBMLStreamSpy` produces,
you may feel more confortable with what is produced here for
stimulations.

The output should look like this :

.. code::

   ...
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 4294967296 (0x100000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 8589934592 (0x200000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 12884901888 (0x300000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 17179869184 (0x400000000) and duration 0 (0x0)
   ...

Now let's try to understand what is produced. Each line represents
received stimulation. The input index which received the stimulation
is printed. Then follow the stimulation code, its date and its duration.
The stimulation name is retrieved from the type manager when correctly
registered. Here, you can see that an \e OVTK_StimulationId_Label_00 is
received every second.

More tests could be done with another clock stimulator with different
timings and stimulation codes. You will want to add inputs to the stimulation
listener box in order to get proper results. For example with one more box
sending \e OVTK_StimulationId_Label_01 every half second, the output would
look like this :

.. code::

   ...
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 2147483648 (0x80000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 4294967296 (0x100000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 4294967296 (0x100000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 6442450944 (0x180000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 8589934592 (0x200000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 8589934592 (0x200000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 10737418240 (0x280000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 12884901888 (0x300000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 12884901888 (0x300000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 15032385536 (0x380000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 17179869184 (0x400000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 1 (0x1) with name Stimulation stream 2 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 17179869184 (0x400000000) and duration 0 (0x0)
   ...

