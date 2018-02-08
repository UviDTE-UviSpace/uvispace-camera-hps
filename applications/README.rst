Applications
============
* ``camera_server``: C/C++ TCP server that permits to send a gray image to a
  remote host. Useful for debugging when no VGA is available.
* ``camera_vga_test``: C/C++ Sets a default configuration in camera_config and resets
  all the video stream. Image output goes to the VGA. It is useful to select the
  proper configuration for the camera.
* ``image_processing_test``: C/C++ application that permits to change the image processing parameters.
  Currently only binarization thresholds can be modified. It is useful to find
  the best combination of parameters for the application if illumination changes.
* ``triangle-detector-server``: It gets the binary image and gray image from the
  hardware and publish them through ZMQ sockets. It also extracts the vertices of
  the triangles in the binary image and publish the using ZMQ sockets. Any remote
  host can suscribe to receibe images or triangles.

The ``include`` folder contains common headers used by the applications.
