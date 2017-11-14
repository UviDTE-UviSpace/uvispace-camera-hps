#include "abstract_server.hpp"

#include <fstream>

typedef uint8_t color_component;

#define IMAGE_HEIGHT 480
#define IMAGE_WIDTH 640

namespace camera_server {
    class camera_server: public abstract_server::abstract_server {
    public:
        camera_server(int port, int image_type);
        ~camera_server();
    protected:
        std::string process_request(std::string request) override;
    private:
        std::string capture_frame();
        std::ifstream uvicamera;
        uint8_t pixel_size;
    };
}
