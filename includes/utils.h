#ifndef UTILS_H
#define UTILS_H

#include "enums.h"

#include <ft2build.h>
#include FT_FREETYPE_H


void increase_volume ();
void decrease_volume ();
void get_volume (char* text, int size);
void pre_load_bitmaps (bitmap_data** bitmap_holder, FT_Face face); 

#endif
