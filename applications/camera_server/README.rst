camera_server
=============

camera_server allows the operation of the uvispace_camera device from remote
hosts.

The server opens a listening TCP socket on port 36000, and processes the text
commands sent to it.

Launching the application
------------------
When launching this application 3 different options can be added:

.. code-block:: bash

   $ ./camera_server --binary #the image obtained from hardware is binary (1-Byte pixels)
   $ ./camera_server --greyscale #the image obtained from hardware is greyscale (1-Byte pixels)
   $ ./camera_server --rgbg #the image obtained from hardware is rgbg (4-Byte pixels with R, G, B and Gray component)

This application needs the uvispace_camera_driver.ko inserted in the system because it gets the
images through the driver. If it is not inserted insert it with:

.. code-block:: bash

   $ insmod uvispace_camera_driver.ko

TCP/IP Command list
--------------------
* ``capture_frame``: Obtain a new 640x480 frame from the camera and send to host.
* ``quit``: Closes the connection.
