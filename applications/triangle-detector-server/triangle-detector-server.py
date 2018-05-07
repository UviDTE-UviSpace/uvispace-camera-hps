import datetime

import numpy
import zmq

import _find_contours
import _polygon
# This server posts in 3 different zmq sockets the gray image, the binary image and the
# triangles (position of cars) obtained from the binary image.

IMG_WIDTH = 640;
IMG_HEIGHT = 468;

def main():
    # Init fps(frames per second) counter
    mean_fps = 0
    # Bind publisher socket
    bin_frame_publisher = zmq.Context.instance().socket(zmq.PUB)
    gray_frame_publisher = zmq.Context.instance().socket(zmq.PUB)
    triangle_publisher = zmq.Context.instance().socket(zmq.PUB)
    #Make the zmq fuffer equal to 1 so when client asks for data only gives last image.
    #Otherwise with a slow client images would go accumulating in zmq buffers produccing
    #a delay in the stream.
    bin_frame_publisher.sndhwm = 1
    gray_frame_publisher.sndhwm = 1
    triangle_publisher.sndhwm = 1
    bin_frame_publisher.bind("tcp://*:33000")
    gray_frame_publisher.bind("tcp://*:34000")
    triangle_publisher.bind("tcp://*:32000")
    f_bin  =  open("/dev/uvispace_camera_bin",  "rb");
    f_gray =  open("/dev/uvispace_camera_gray", "rb");
    t1 = datetime.datetime.now()
    while True:
        #get last binary image from memory, calculate triangles and serve them
        bin_frame = numpy.fromfile(f_bin, numpy.uint8, IMG_HEIGHT * IMG_WIDTH).reshape((IMG_HEIGHT, IMG_WIDTH))
        triangles = process_frame(bin_frame)
        triangle_publisher.send_json(triangles)
        #get gray image from memory and serve binary and gray images
        bin_frame_publisher.send(bin_frame)
        gray_frame = numpy.fromfile(f_gray, numpy.uint8, IMG_HEIGHT * IMG_WIDTH).reshape((IMG_HEIGHT, IMG_WIDTH))
        gray_frame_publisher.send(gray_frame)
        #calculate frame rate
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
