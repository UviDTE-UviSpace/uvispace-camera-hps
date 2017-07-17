#include "camera_server.hpp"

int main() {
    camera_server::camera_server cs(36000);
    cs.run();
    return 0;
}
