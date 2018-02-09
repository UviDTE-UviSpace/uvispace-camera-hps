Triangle Detector Server
========================

Python application that gets the binary image and gray image from the
hardware and publish them through ZMQ sockets. It also extracts the vertices of
the triangles in the binary image and publish them using another ZMQ socket. Any remote
host can suscribe to receive images or triangles. The sockets are the following:

  * Port 32000: triangles vertices.
  * Port 33000: 640x468 binary image (8-Byte/pixel).
  * Port 34000: 640x468 gray image (8-Byte/pixel).

This image gets a 640x480 image from hardware but deletes the last 12 lines because they
are black. In Uvispace this black lines are a problem because the prevent from making a
continuous image joining the images from various cameras. Further investigation on the
camera settings must be done to obtain a full 640x480 image with all active pixels.

Launching the application
-------------------------

First of all you must create a virtual environment called *uvicamera* and work in it:

.. code-block:: bash

   $ workon uvicamera

Then install the required libraries inside it:

.. code-block:: bash

   $ pip install -r requirements.txt

This application needs the uvispace_camera_driver.ko inserted in the system because it gets the
images through the driver. If it is not inserted insert it with:

.. code-block:: bash

  $ insmod uvispace_camera_driver.ko

Launch the application with:

.. code-block:: bash

   $ python triangle-detector-server.py
