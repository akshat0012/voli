#include <stdint.h>
#include <xcb/xcb.h>

#include "enums.h"
#include "config.h"
#include "render_text.h"

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

void render_text( xcb_connection_t *conn,
                  xcb_drawable_t wid,
                  xcb_gcontext_t gc,
                  uint8_t depth,
                  bitmap_data** bitmap_holder,
                  char *text ) 
{
    printf("\n");
    int len = strlen(text);
    printf("text -> %s\n", text);

    uint32_t x_offset = 17;
    uint32_t xcb_bitmap_rows = 0;
    uint32_t xcb_bitmap_width = 0;

    for (int i=0; i<len; i++) {
        xcb_bitmap_rows = MAX(xcb_bitmap_rows, bitmap_holder[text[i] - '0']->rows);
        xcb_bitmap_width += bitmap_holder[text[i] - '0']->width + x_offset;
        printf("text[%c]->width = %d\n", text[i], bitmap_holder[text[i] - '0']->width);
    }

    uint32_t *xcb_bitmap = calloc(xcb_bitmap_rows * xcb_bitmap_width, sizeof(uint32_t));

    int cursor = 0;    
    for ( int c=0; c<len; c++ ) {
        printf("char(%c) is being copied now \n", text[c]);
        
        int index = text[c] - '0';

        for ( unsigned int row=0; row<bitmap_holder[index]->rows; row++ ) {
            for ( unsigned int col=0; col<bitmap_holder[index]->width; col++ ) {
                int new_writing_pos = cursor + col;
                xcb_bitmap[row * xcb_bitmap_width + new_writing_pos] = 
                    bitmap_holder[index]->data[row * bitmap_holder[index]->width + col] ? TEXT_COLOR : 0x0;
            }
        }
        cursor += x_offset;
    }

    
    printf("Copying Complete:: Sending for xcb_put_image()\n");
    printf("Printing Complete Bitmap\n");
    printf("xcb_bitmap[%d][%d]\n", xcb_bitmap_rows, xcb_bitmap_width);

    for ( unsigned int row=0; row<xcb_bitmap_rows; row++ ) {
        for ( unsigned int col=0; col<xcb_bitmap_width; col++ ) {
            printf("%c ", xcb_bitmap[row * xcb_bitmap_width + col] ? '@' : ' ');
        }
        printf("\n");
    }

    xcb_void_cookie_t cookie = xcb_put_image(
        conn,
        XCB_IMAGE_FORMAT_Z_PIXMAP,
        wid,
        gc,
        xcb_bitmap_width,
        xcb_bitmap_rows,
        5, 5,
        0,
        depth,
        xcb_bitmap_rows * xcb_bitmap_width * sizeof(uint32_t),
        (uint8_t *)xcb_bitmap
    );

    xcb_generic_error_t *error = xcb_request_check (conn, cookie);
    if (error) {
        printf("ERROR::xcb_put_image()\n");
    }
    free(xcb_bitmap);
    xcb_bitmap = NULL;
}
