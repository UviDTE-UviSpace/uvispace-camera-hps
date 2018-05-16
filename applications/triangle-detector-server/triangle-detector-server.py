import datetime

import numpy
import zmq
import sys

import _find_contours
import _polygon
# This server posts in 3 different zmq sockets:
# the gray or rgb image is posted in port 34000
# the binary image is posted in socket 33000
# triangles vertices (position of UGVs) are posted in socket32000
# When calling with no arguments gray image is sent by default.
# Use the custom resolution call adding RGB at the end to send the
# RGB image instead of the GRAY

IMG_WIDTH_DEFAULT = 640;
IMG_HEIGHT_DEFAULT = 480;
LINES_SKIP_DEFAULT = 12; #lines not sent via internet
IMAGE_SEND_DEFAULT = "GRAY"

def main():
    #Check the number of arguments passed to set resolution
    if len(sys.argv)==1:
        IMG_WIDTH = IMG_WIDTH_DEFAULT
        IMG_HEIGHT = IMG_HEIGHT_DEFAULT
        LINES_SKIP = LINES_SKIP_DEFAULT
        IMAGE_SEND = IMAGE_SEND_DEFAULT
    elif len(sys.argv)==5:
        IMG_WIDTH = int(sys.argv[1])
        IMG_HEIGHT = int(sys.argv[2])
        LINES_SKIP = int(sys.argv[3])
        if (sys.argv[4]) == "RGB":
            IMAGE_SEND = "RGB"
        else:
            IMAGE_SEND = IMAGE_SEND_DEFAULT
    else:
        print 'For custom resolution call:'
        print '  python triangle_detector_server.py width height lines_skip image_type'
        print 'Example getting 1280x960 image from hardware and sending binary and gray skipping 24 lines:'
        print '  python triangle_detector_server.py 1280 960 24 GRAY'
        print 'Example getting 1280x960 image from hardware and sending binary and rgb skipping 24 lines:'
        print '  python triangle_detector_server.py 1280 960 24 RGB'
        print 'Call without arguments for default Uvispace: 640x480 skip last 12 lines sending gray image'
        return

    #set the resolution in the driver
    f_width  =  open("/sys/uvispace_camera/attributes/image_width", "w");
    file.write(f_width, str(IMG_WIDTH));
    file.close(f_width);
    f_height  =  open("/sys/uvispace_camera/attributes/image_height", "w");
    file.write(f_height, str(IMG_HEIGHT));
    file.close(f_height);

    # Bind publisher socket
    bin_frame_publisher = zmq.Context.instance().socket(zmq.PUB)
    rgbgray_frame_publisher = zmq.Context.instance().socket(zmq.PUB)
    triangle_publisher = zmq.Context.instance().socket(zmq.PUB)
    #Make the zmq fuffer equal to 1 so when client asks for data only gives last image.
    #Otherwise with a slow client images would go accumulating in zmq buffers produccing
    #a delay in the stream.
    bin_frame_publisher.sndhwm = 1
    rgbgray_frame_publisher.sndhwm = 1
    triangle_publisher.sndhwm = 1
    bin_frame_publisher.bind("tcp://*:33000")
    rgbgray_frame_publisher.bind("tcp://*:34000")
    triangle_publisher.bind("tcp://*:32000")
    f_bin  =  open("/dev/uvispace_camera_bin",  "rb");
    if IMAGE_SEND == "GRAY":
        f_rgbgray =  open("/dev/uvispace_camera_gray", "rb");
    else:
        f_rgbgray =  open("/dev/uvispace_camera_rgbg", "rb");

    # Start loop
    IMG_HEIGHT_SEND = IMG_HEIGHT-LINES_SKIP;
    mean_fps = 0
    t1 = datetime.datetime.now()
    while True:
        #extract triangles from bin image and publish them
        bin_frame = numpy.fromfile(f_bin, numpy.uint8, IMG_HEIGHT_SEND * IMG_WIDTH).reshape((IMG_HEIGHT_SEND, IMG_WIDTH))
        triangles = process_frame(bin_frame)
        triangle_publisher.send_json(triangles)
        #publish binary image and gray image
        bin_frame_publisher.send(bin_frame)
        if IMAGE_SEND == "GRAY":
            rgbgray_frame = numpy.fromfile(f_rgbgray, numpy.uint8, IMG_HEIGHT_SEND * IMG_WIDTH).reshape((IMG_HEIGHT_SEND, IMG_WIDTH))
        else:#RGB
            rgbgray_frame = numpy.fromfile(f_rgbgray, numpy.uint32, IMG_HEIGHT_SEND * IMG_WIDTH).reshape((IMG_HEIGHT_SEND, IMG_WIDTH))
        rgbgray_frame_publisher.send(rgbgray_frame)
        #update frame rate and print
        t2 = datetime.datetime.now()
        loop_time = (t2 - t1).microseconds
        t1 = t2
        last_fps = 1000000 / loop_time
        print(last_fps)


def process_frame(frame):
    # Get shapes from image
    triangles = get_shapes(frame)
    return triangles


def get_shapes(image):
    triangles = []
    contours = _find_contours.find_contours(image, 0.9)
    for contour in contours:
        coords = _polygon.approximate_polygon(contour, 8)
        vertices = len(coords)
        if vertices == 3 and not numpy.array_equal(coords[0], coords[-1]):
            triangle = coords[:].tolist()
            triangles.append(triangle)
        elif vertices == 4 and numpy.array_equal(coords[0], coords[-1]):
            triangle = coords[1:].tolist()
            triangles.append(triangle)
    return triangles


if __name__ == '__main__':
    main()
