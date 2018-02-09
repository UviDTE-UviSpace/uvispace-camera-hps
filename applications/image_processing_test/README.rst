image_processing_test
=====================

Changes the image processing values on the flight. It doesnt affect the video stream
at all so you can play with this application and change the image processing. Currently
only the binarization thresholds applied to the HSV image can be modified. For example
if you change the hue threshold you can binarize for a different color.

Launching the application
------------------
If you call this applicattion without extra parameters default image processing settings are applied.

.. code-block:: bash

   $ ./img_processing_test

To change values from console add the parameters values when calling. To see an example
print the help message, copy the example in the help message and modify the values:
To get the help message:

.. code-block:: bash

   $ ./img_processing_test -h
