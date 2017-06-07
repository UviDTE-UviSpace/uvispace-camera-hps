#include "abstract_server.hpp"

#include "avalon_camera.hpp"
#include "hps_0.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <stdlib.h>
#include <algorithm>

#define HPS_FPGA_BRIDGE_BASE 0xC0000000
#define HW_REGS_BASE (HPS_FPGA_BRIDGE_BASE)
#define HW_REGS_SPAN (0x04000000)
#define HW_REGS_MASK (HW_REGS_SPAN - 1)

// Constants to mmap HPS On-Chip Ram (used to capture images)
#define HPS_OCR_BASE 0xFFFF0000
#define HPS_OCR_SPAM (64*1024) //64kB
#define HPS_OCR_MASK (HPS_OCR_SPAM - 1)

//Image properties
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

class camera_server: public abstract_server {
public:
    camera_server(int port);
protected:
    std::string process_request(std::string request);
private:
    std::string get_image();
};
