========================
HPS software scripts
========================
This folder contains the C/C++ applications to run in the processor of the Cyclone V SoC.
* inc:common header files in

* test_programs: programs to individually test some parts of the program. 
	* camera_test: it sets a configuration in camera_config a resets all the video stream. Image goes to the VGA. It is useful to select the proper configuration for the camera.
	* LEDs_program: 1 LED is switched ON in sequence left to right and right to left. It is the most basic example of access to the FPGA from the HPS using mmap.

* camera_control_socket: this is the main program for uvirobot providing full control of the board through internet sockets.