import datetime

import numpy
import zmq


def main():
    # Configure sysfs variables
    with open("/sys/uvispace_camera/attributes/image_type", "w") as f:
        f.write("2")
    # Init fps counter
    mean_fps = 0
    # Bind publisher socket
    publisher = zmq.Context.instance().socket(zmq.PUB)
    publisher.bind("tcp://*:33000")
    while True:
        t1 = datetime.datetime.now()
        frame = process_frame()
        publisher.send(frame)
        last_fps = 1000000 / (datetime.datetime.now() - t1).microseconds
        mean_fps = 0.9 * mean_fps + 0.1 * last_fps
        # print(mean_fps)


def process_frame():
    # Get binarized image
    with open("/dev/uvispace_camera", "rb") as f:
        frame = numpy.fromfile(f, numpy.uint8, 480 * 640).reshape((480, 640))
    return frame


if __name__ == '__main__':
    main()
