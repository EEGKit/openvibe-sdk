.. _Doc_BoxAlgorithm_StimulationMultiplexer:

Stimulation multiplexer
=======================

.. container:: attribution

   :Author:
      Yann Renard
   :Company:
      INRIA/IRISA


.. image:: images/Doc_BoxAlgorithm_StimulationMultiplexer.png

The stimulations are ordered according to their start date. Thus each time all the input have chunks covering a period of time, a new output chunk is sent. This box may eventually produce output chunk reflecting a different duration depending on the inputs.

The Stimulation multiplexer box algorithm merges several stimulation streams
into one stimulation stream. The contained stimulations are ordered
according to their start date. Thus each time all the input have chunks
covering a period of time, a new output chunk is sent. This box may eventually
produce output chunk reflecting a different duration depending on the inputs.

Inputs
------

.. csv-table::
   :header: "Input Name", "Stream Type"

   "Input stimulations 1", "Stimulations"
   "Input stimulations 2", "Stimulations"

This box can receive as many input as necessary. All the inputs
will be of type :ref:`Doc_Streams_Stimulation` in order to
be parsed by this the reader. Every input have to be connected
for this box to work correctly

Input stimulations 1
~~~~~~~~~~~~~~~~~~~~

This is the first default input of this box.

Input stimulations 2
~~~~~~~~~~~~~~~~~~~~

This is the second default input of this box.

Outputs
-------

.. csv-table::
   :header: "Output Name", "Stream Type"

   "Multiplexed stimulations", "Stimulations"

Multiplexed stimulations
~~~~~~~~~~~~~~~~~~~~~~~~

The output of this box is a new :ref:`Doc_Streams_Stimulation`
that contains all the stimulation of respective inputs. The
input streams are multiplexed in a single stream ordering the
stimulations according to their starting time.

.. _Doc_BoxAlgorithm_StimulationMultiplexer_Examples:

Examples
--------

To illustrate this box, just drag'n drop a :ref:`Doc_BoxAlgorithm_KeyboardStimulator`,
a :ref:`Doc_BoxAlgorithm_ClockStimulator` and a :ref:`Doc_BoxAlgorithm_StimulationListener`
box. Connect the two stimulation generators to your stimulation
multiplexer and connect the output of the stimulation multiplexer
to the stimulation listener. Leave the default parameters for all
of those boxes, except the clock stimulator which you'll configure
with an \e OVTK_StimulationId_Beep stimulation. Now pres 'start'.

You should see something like this :

.. code::

   ...
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33282 (0x8202)[OVTK_StimulationId_Beep] at date 4294967296 (0x100000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33025 (0x8101)[OVTK_StimulationId_Label_01] at date 8187281408 (0x1e8000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33282 (0x8202)[OVTK_StimulationId_Beep] at date 8589934592 (0x200000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 8992587776 (0x218000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33026 (0x8102)[OVTK_StimulationId_Label_02] at date 12213813248 (0x2d8000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33282 (0x8202)[OVTK_StimulationId_Beep] at date 12884901888 (0x300000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33282 (0x8202)[OVTK_StimulationId_Beep] at date 17179869184 (0x400000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 18924699648 (0x468000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33282 (0x8202)[OVTK_StimulationId_Beep] at date 21474836480 (0x500000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33027 (0x8103)[OVTK_StimulationId_Label_03] at date 23756537856 (0x588000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33024 (0x8100)[OVTK_StimulationId_Label_00] at date 24561844224 (0x5b8000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33282 (0x8202)[OVTK_StimulationId_Beep] at date 25769803776 (0x600000000) and duration 0 (0x0)
   [  INF  ] <Box algorithm::Stimulation listener> For input 0 (0x0) with name Stimulation stream 1 got stimulation 33282 (0x8202)[OVTK_StimulationId_Beep] at date 30064771072 (0x700000000) and duration 0 (0x0)
   ...

In this session, I pressed \e z, \e z and \e a successively, causing
an \e OVTK_StimulationId_Label_0x stimulation to be sent at key pressed
time, and an \e OVTK_StimulationId_Label_00 stimulation to be sent at key
release time. Inserted in those stimulations are several \e OVTK_StimulationId_Beep
stimulations coming from the clock stimulator.

