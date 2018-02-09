#include "main.hpp"

#define PORT 36000

int main(int argc, char** argv) {
    // Process command line arguments
    if (argc != 2) {
      std::cout << "Usage:\n";
      std::cout << "camera_server --binary\n";
      std::cout << "camera_server --greyscale\n";
      std::cout << "camera_server --rgbg\n";
      return 1;
    }

    std::string image_type_argument(argv[1]);
    int image_type;
    if (image_type_argument == "--rgbg") {
      image_type = 0;
    } else if (image_type_argument == "--greyscale") {
      image_type = 1;
    } else if (image_type_argument == "--binary") {
      image_type = 2;
    } else {
      std::cout << "Usage:\n";
      std::cout << "camera_server --binary\n";
      std::cout << "camera_server --greyscale\n";
      std::cout << "camera_server --rgbg\n";
      return 1;
    }

    // Run server
    camera_server::camera_server cs(PORT, image_type);
    cs.run();
    return 0;
}
