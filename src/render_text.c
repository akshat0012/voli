#include <stdint.h>
#include <xcb/xcb.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "render_text.h"
#include "config.h"

void render_text(xcb_connection_t *conn,
                xcb_drawable_t wid,
                xcb_gcontext_t gc,
                uint8_t depth,
                FT_Face face,
                char *text) 
{


    int len = strlen(text);
    int x_offset = 10;

    for (int i=0; i<len; i++) {
        if (FT_Load_Char(face, text[i], FT_LOAD_RENDER)) {
            printf("ERROR::FT_Load_Char()\n");
        }

        FT_Bitmap* bitmap = &face->glyph->bitmap;

        uint32_t *xcb_bitmap = calloc(bitmap->width * bitmap->rows, sizeof(uint32_t));

        for (unsigned int y = 0; y < bitmap->rows; y++) {
            for (unsigned int x = 0; x < bitmap->width; x++) {
                xcb_bitmap[y * bitmap->width + x] = bitmap->buffer[y * bitmap->width + x] ? TEXT_COLOR : 0x0;
            }
        }

        xcb_void_cookie_t cookie = xcb_put_image(
            conn,
            XCB_IMAGE_FORMAT_Z_PIXMAP,
            wid,
            gc,
            bitmap->width,
            bitmap->rows,
            (x_offset), (HEIGHT-face->glyph->bitmap_top-15),
            0,
            depth,
            bitmap->width * bitmap->rows * sizeof(uint32_t),
            (uint8_t *)xcb_bitmap
        );

        x_offset += 24;

        xcb_generic_error_t *error = xcb_request_check (conn, cookie);
        if (error) {
            printf("ERROR::xcb_put_image()\n");
        }
        free(xcb_bitmap);
    }
}
