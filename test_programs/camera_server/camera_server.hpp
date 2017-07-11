#include "abstract_server.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

typedef uint8_t color_component;

#define IMAGE_HEIGHT 480
#define IMAGE_WIDTH 640
#define IMAGE_MEM (sizeof(uint8_t) * 4 * IMAGE_HEIGHT * IMAGE_WIDTH)

namespace camera_server {
    class camera_server: public abstract_server {
    public:
        camera_server(int port);
    protected:
        std::string process_request(std::string request);
    private:
        std::string capture_frame();
    };
}
