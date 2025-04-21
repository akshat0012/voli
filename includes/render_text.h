#ifndef TEXT_H
#define TEXT_H

#include <xcb/xcb.h>
#include "enums.h"

void render_text ( xcb_connection_t *conn,
                   xcb_drawable_t wid,
                   xcb_gcontext_t gc,
                   uint8_t depth,
                   bitmap_data** bitmap_data,
                   char *text );
#endif
