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
    char* buffer = new char[IMAGE_MEM];
    myfile.open("/dev/uvispace_camera", std::ios::binary);
    if (!myfile.is_open()) {
        std::cout << "Can't open file\n";
        return buffer;
    }
    myfile.read(buffer, IMAGE_MEM);
    myfile.close();

    std::string result = "";
    for (uint32_t i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT*4; i += 1) {
        result += buffer[i];
    }
    return result;
}
