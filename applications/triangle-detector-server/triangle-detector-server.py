import datetime

import numpy
import zmq

import _find_contours
import _polygon
# This server posts in 3 different zmq sockets the gray image, the binary image and the
# triangles (position of cars) obtained from the binary image.

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
    gray_frame_publisher.bind("tcp://*:33001")
    triangle_publisher.bind("tcp://*:5005")
    f_bin  =  open("/dev/uvispace_camera_bin",  "rb");
    f_gray =  open("/dev/uvispace_camera_gray", "rb");
    while True:
        t1 = datetime.datetime.now()
        bin_frame = numpy.fromfile(f_bin, numpy.uint8, 480 * 640).reshape((480, 640))
        triangles = process_frame(bin_frame)
        triangle_publisher.send_json(triangles)
        bin_frame_publisher.send(bin_frame)
        gray_frame = numpy.fromfile(f_gray, numpy.uint8, 480 * 640).reshape((480, 640))
        gray_frame_publisher.send(gray_frame)
        last_fps = 1000000 / (datetime.datetime.now() - t1).microseconds
        mean_fps = 0.9 * mean_fps + 0.1 * last_fps
        print(mean_fps)


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
