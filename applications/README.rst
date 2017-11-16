Applications
============
* ``camera_capture_test``: Captures an image from the camera and outputs its
  pixel components in the terminal screen.
* ``camera_server``: Acts as a TCP server to allow the operation of the camera
  from remote hosts.
* ``camera_vga_test``: Sets a default configuration in camera_config and resets
  all the video stream. Image output goes to the VGA. It is useful to select the
  proper configuration for the camera.
* ``image_processing_test``: It permits to change the image processing parameters.
  Currently only binarization thresholds can be modified. It is useful to find
  the best combination of parameters for the application if illumination changes.
* ``LEDs_program``: 1 LED is switched on in sequence left to right and right to
  left. It is the most basic example of access to the FPGA from the HPS using
  mmap.

The ``include`` folder contains common headers used by the applications.
