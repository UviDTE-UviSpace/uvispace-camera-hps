camera_server
=============

camera_server allows the operation of the uvispace_camera device from remote
hosts.

The server opens a listening TCP socket on port 36000, and processes the text
commands sent to it.

Command list
------------
* ``capture_frame``: Obtain a new frame from the camera.
* ``quit``: Closes the connection.
