#ifndef ENUMS_H
#define ENUMS_H

#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    FT_Library library;
    FT_Face    face;
    int        set_pixel;
} FT_Context;

typedef struct {
    uint32_t* data;
    uint32_t rows;
    uint32_t width;
} bitmap_data;

#endif
