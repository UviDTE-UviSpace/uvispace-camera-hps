#include <inttypes.h>

/*
Macros for accessing an address map with a data bus size of 8, 16 or 32 bits.
*/
#ifndef __LOW_LEVEL_RW_MACROS
#define __LOW_LEVEL_RW_MACROS
// Macro for R/W operations on 8-bit addresses.
#define IOWR8(base, offset, dat)        (*((uint8_t*)(base) + (offset)) = (uint8_t)(dat))
#define IORD8(base, offset)             (*((uint8_t*)(base) + (offset)))
// Macro for R/W operations on 16-bit addresses.
#define IOWR16(base, offset, dat)       ((* ((uint16_t*)((uint8_t*)(base) + (offset)*2))) = (uint16_t)(dat))
#define IORD16(base, offset)            (* ((uint16_t*)((uint8_t)(base) + (offset)*2)))
// Macro for R/W operations on 32-bit addresses.
#define IOWR32(base, offset, dat)       ((* ((uint32_t*)((uint8_t*)(base) + (offset)*4))) = (uint32_t)(dat))
#define IORD32(base, offset)            (* ((uint32_t*)((uint8_t*)(base) + (offset)*4)))
#endif
