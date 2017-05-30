====================
HPS software scripts
====================
This folder contains the C/C++ applications to run in the processor of the Cyclone V SoC.

* inc: Common header files used all HPS programs.

* test_programs: Programs to individually test some parts of the program.

	* camera_vga_test: Sets a configuration in camera_config and resets all the video stream. Image output goes to the VGA. It is useful to select the proper configuration for the camera.
	* camera_capture_test: Captures an image from the camera and outputs its pixel components in the terminal screen.
	* LEDs_program: 1 LED is switched ON in sequence left to right and right to left. It is the most basic example of access to the FPGA from the HPS using mmap.
