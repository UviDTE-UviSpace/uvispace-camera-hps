#include "camera_server.hpp"

int main(int argc, char **argv) {
    camera_server::camera_server cs = camera_server::camera_server(36000);
    cs.run();
    return 0;
}
