#ifndef RGB_DIGITAL_H
#define RGB_DIGITAL_H

#include "pico/stdlib.h"

#define URGB_U32(r, g, b) \
    ((uint32_t)((r) << 8) | \
     (uint32_t)((g) << 16) | \
     (uint32_t)(b))

#define U32_RED URGB_U32(0xFF, 0, 0)
#define U32_YELLOW URGB_U32(0xFF, 0xFF, 0)
#define U32_GREEN URGB_U32(0, 0xFF, 0)

void init_RGB();
void change_color(uint32_t color);

#endif
 