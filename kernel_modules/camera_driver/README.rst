camera_driver
=============

Character device driver for the uvispace_camera. It creates a readable device
to allow frame capture by user space applications.

Compilation and installation
----------------------------
To compile this kernel module, it is necessary to use the kernel headers
available in the SDK. To this end, the SDKROOT environment variable should be
set pointing to the root folder of the SDK.

.. code-block:: shell

  export SDKROOT =  <root folder of SDK>
  make build


The resulting uvispace_camera_driver.ko file should be installed in the target
system with root permission. If successful, a entry for the device should be
created in dev directory.

.. code-block:: shell

  insmod uvispace_camera_driver.ko
  ls /dev/uvispace_camera
