camera_vga_test
================

Changes the values of the camera settings of the camera and resets it so the
changes make effect. (The softreset doenst work properly so a hardreset with
a key in the board must be done after executing this application)

Launching the application
------------------
If you call this applicattion without extra parameters default camera settings are applied.

.. code-block:: bash

   $ ./camera_vga_test

To change values from console add the parameters values when calling. To see an example
print the help message, copy the example in the help message and modify the values:
To get the help message:

.. code-block:: bash

   $ ./camera_vga_test -h
