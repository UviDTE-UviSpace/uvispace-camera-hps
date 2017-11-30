import datetime

import numpy
import zmq

import _find_contours
import _polygon


def main():
    # Configure image type as binary
    with open("/sys/uvispace_camera/attributes/image_type", "w") as f:
        f.write("2")
    # Init fps counter
    mean_fps = 0
    # Bind publisher socket
    publisher = zmq.Context.instance().socket(zmq.PUB)
    publisher.bind("tcp://*:5005")
    while True:
        t1 = datetime.datetime.now()
        triangles = process_frame()
        publisher.send_json(triangles)
        last_fps = 1000000 / (datetime.datetime.now() - t1).microseconds
        mean_fps = 0.9 * mean_fps + 0.1 * last_fps
        # print(mean_fps)


def process_frame():
    # Get binarized image
    with open("/dev/uvispace_camera", "rb") as f:
        frame = numpy.fromfile(f, numpy.uint8, 480 * 640).reshape((480, 640))
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
