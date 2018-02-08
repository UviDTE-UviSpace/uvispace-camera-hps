camera_driver
=============

Character device driver for the uvispace_camera. It creates uvispace_camera_bin,
uvispace_camera_gray and uvispace_camera_rgbg in /dev that can be used to read
a binary, gray or RGB + Gray images respectively from the FPGA Video Stream.
Applications can make use of the 3 as independent devices and read images
from camera using the regular file operators open, read and close. Instances
to control some aspects of the driver are also created in /sys/uvispace_camera.

Compilation and installation
----------------------------
To compile this kernel module, it is necessary to use the kernel headers
available in the SDK obtained when compiling the Operating System.
To this end, the SDKROOT environment variable should be
set pointing to the root folder of the SDK.

.. code-block:: shell

  export SDKROOT =  <root folder of SDK>
  make build


The resulting uvispace_camera_driver.ko file should be installed in the target
system with root permission. If successful, a entry for the device should be
created in dev directory.

.. code-block:: shell

  insmod uvispace_camera_driver.ko
  ls /dev/uvispace_camera*
  ls /sys/uvispace_camera/attributes
