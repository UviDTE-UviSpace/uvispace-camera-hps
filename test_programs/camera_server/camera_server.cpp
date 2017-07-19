#include "camera_server.hpp"

camera_server::camera_server::camera_server(int port) : abstract_server(port) {
    // Open camera device
    this->uvicamera.open("/dev/uvispace_camera", std::ios::binary);
    if (!this->uvicamera.is_open()) {
        throw server_init_error("uvispace_camera could not be open");
    }
}

camera_server::camera_server::~camera_server() {
    this->uvicamera.close();
}

std::string camera_server::camera_server::process_request(std::string request) {
    if (request == "capture_frame") {
        return this->capture_frame();
    }
    return abstract_server::process_request(request);
}

std::string camera_server::camera_server::capture_frame() {
    std::string result(IMAGE_MEM, '\0');
    this->uvicamera.read(&result[0], IMAGE_MEM);
    return result;
}
