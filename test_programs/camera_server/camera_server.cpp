#include "camera_server.hpp"

camera_server::camera_server::camera_server(int port) : abstract_server(port) {}

std::string camera_server::camera_server::process_request(std::string request) {
    if (request == "capture_frame") {
        return this->capture_frame();
    }
    return abstract_server::process_request(request);
}

std::string camera_server::camera_server::capture_frame() {
    std::ifstream myfile;
    myfile.open("/dev/uvispace_camera", std::ios::binary);
    if (!myfile.is_open()) {
        std::cout << "File is closed\n";
        return "";
    }
    std::string result(IMAGE_MEM, '\0');
    myfile.read(&result[0], IMAGE_MEM);
    myfile.close();
    return result;
}
