//file: avalon_camera.c

#include <inttypes.h>//for uint16_t
#include "avalon_camera.h"

static void* base_address; //address of the camera component in the processors address map

int camera_set_exposure(uint16_t exposure)
{
    IOWR_CAMERA_EXPOSURE(base_address, exposure);
    return 0;
}
int camera_set_row_size(uint16_t row_size)
{
    IOWR_CAMERA_ROW_SIZE(base_address, row_size);
    return 0;
}
int camera_set_col_size(uint16_t col_size)
{
    IOWR_CAMERA_COLUMN_SIZE(base_address, col_size);
    return 0;
}
int camera_generate_soft_reset(void)
{
    IOWR_CAMERA_CAMERA_SOFT_RESET(base_address, 0);//reset
    IOWR_CAMERA_CAMERA_SOFT_RESET(base_address, 1);//remove reset
    return 0; //return 0 on success
}

int camera_init(void* camera_address)
{
    base_address = camera_address;
    
    return 0; //return 0 on success
}