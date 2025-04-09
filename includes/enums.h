#ifndef ENUMS_H
#define ENUMS_H

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    FT_Library library;
    FT_Face    face;
    int        set_pixel;
} FT_Context;



#endif
