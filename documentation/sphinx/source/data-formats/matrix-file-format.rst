.. _Doc_MatrixFileFormat:

Matrix File Format
==================

Sometimes it is useful to be able to save/load OpenViBE matrices to/from disk.
Examples of such matrices are channel localization data that contains a list of
electrode names and their coordinates, and color scales, which may be used by
visualisation plugins to map potentials to colors. To that end, an OpenViBE
matrix file format was defined and OpenViBE matrix file reader/writer
algorithms created to process it. For a programmer, either the algorithms can
be used, or more straightforwardly via the toolkit functions,

.. code::

   OpenViBE::CMatrix.toTextFile(...)
   OpenViBE::CMatrix.fromTextFile(...)

On disk, this format follows a very simple text syntax for maximum portability.

.. note::

  Since the matrices are stored as text, they do not maintain absolute float
  precision. For the aforementioned save routine, a desired precision can be
  given.

Comments may be inserted anywhere, start with a ``#`` character and extend to
the end of the line.

Also, trailing/leading whitespaces on a line are trimmed by the parser.

The file should begin by the header section, delimited by opening and closing
brackets. Each dimension of the matrix is listed in turn and delimited by its
own opening and closing brackets. Dimension element labels should be declared
within these brackets, each being delimited by quotation marks. Empty labels
may be used, and the number of labels must match the dimension size. This is
what a header might look like :

::

   # header
   [
           [ "color1" "color2" ]
           [ "Red" "Green" "Blue" ]
   ]

The next section of the file is the buffer part of the matrix. Opening and closing brackets are used to identify dimensions. Elements are listed in the innermost dimension, and are separated by quotation marks. This is what an appropriate buffer could look like for a 2×3 matrix :

::

   # header
   [
           [ "color1" "color2" ]
           [ "Red" "Green" "Blue" ]
   ]
   
   #buffer
   
   [ 
           [1 0 0] #color1 red
           [0 1 0] #color2 blue
   ]
   
   #end of buffer
