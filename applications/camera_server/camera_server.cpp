#include "camera_server.hpp"

camera_server::camera_server::camera_server(int port, int image_type) : abstract_server(port) {
    // Store pixel size in Bytes
    if (image_type == 0) {
      this->pixel_size = 4;
    } else if ((image_type == 1) || (image_type == 2)) {
      this->pixel_size = 1;
    }

    // Open camera device
    if (image_type == 0){
        this->uvicamera.open("/dev/uvispace_camera_rgbg", std::ios::binary);
    } else if (image_type == 1) {
        this->uvicamera.open("/dev/uvispace_camera_gray", std::ios::binary);
    } else if (image_type == 2) {
        this->uvicamera.open("/dev/uvispace_camera_bin", std::ios::binary);
    }

    if (!this->uvicamera.is_open()) {
        throw server_error::server_init_error("uvispace_camera could not be open");
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
    std::uint32_t image_size = IMAGE_HEIGHT * IMAGE_WIDTH * this->pixel_size;
    std::string result(image_size, '\0');
    this->uvicamera.read(&result[0], image_size);
    return result;
}
