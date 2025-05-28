#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>

#include <xcb/xcb.h>

#include "enums.h"
#include "utils.h"
#include "config.h"
#include "render_text.h"
#include "init_freetype.h"

int main() {
    


    xcb_connection_t *connection = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(connection)) {
        printf("Error: Can't connect to X server.\n");
        return -1;
    }

    const xcb_setup_t *setup = xcb_get_setup(connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen = iter.data;
    xcb_window_t root = screen->root;

    xcb_window_t wid = xcb_generate_id(connection); 
    uint32_t masks = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_OVERRIDE_REDIRECT;

    FT_Context* main_p_fctx = init_freetype();

    bitmap_data** bitmap_holder = malloc(10 * sizeof(bitmap_data*));
    pre_load_bitmaps (bitmap_holder, main_p_fctx->face);

    FT_Done_Face(main_p_fctx->face);
    FT_Done_FreeType(main_p_fctx->library);
    free(main_p_fctx);

    uint32_t* main_p_value_list = malloc(3*sizeof(uint32_t));

    main_p_value_list[0] = 0x000000;
    main_p_value_list[1] = BORDER_COLOR;
    main_p_value_list[2] = 1;



    xcb_create_window(connection,
                      XCB_COPY_FROM_PARENT,
                      wid,
                      root,
                      POS_X,
                      POS_Y,
                      WIDTH,
                      HEIGHT,
                      1,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      masks,
                      main_p_value_list);

    main_p_value_list[0] = 0xFFFFFF;
    main_p_value_list[1] = 0x000000;

    xcb_gcontext_t gc = xcb_generate_id(connection);
    xcb_create_gc(connection, gc, wid, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND, main_p_value_list);

    free(main_p_value_list);

    char text[5];
    int size_of_text = sizeof(text);

    xcb_grab_key(connection, 1, screen->root, 0, VOLUME_UP_KEYCODE,
                 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    xcb_grab_key(connection, 1, screen->root, 0, VOLUME_DOWN_KEYCODE,
                 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    xcb_flush(connection);

    int visible = 0;
    xcb_generic_event_t *event;
    time_t last_shown = 0;

    while (1) {
        event = xcb_poll_for_event(connection);
        if (event) {

            if ((event->response_type & ~0x80) == XCB_KEY_RELEASE) {
                continue;
            }
            if ((event->response_type & ~0x80) == XCB_KEY_PRESS) {
                xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
                if (visible) {
                    xcb_unmap_window (connection, wid);
                }
                if (kp->detail == VOLUME_UP_KEYCODE) {
                    xcb_map_window(connection, wid);
                    increase_volume();
                    get_volume ( text, 
                                 size_of_text );
                    render_text ( connection,
                                  wid, 
                                  gc,
                                  screen->root_depth,
                                  bitmap_holder,
                                  text );

                    xcb_flush(connection);
                    last_shown = time(NULL);
                    visible = 1;
                }
                else if (kp->detail == VOLUME_DOWN_KEYCODE) {
                    xcb_map_window(connection, wid);
                    decrease_volume();
                    get_volume ( text, 
                                 size_of_text );
                    render_text ( connection,
                                  wid, 
                                  gc,
                                  screen->root_depth,
                                  bitmap_holder,
                                  text );

                    xcb_flush(connection);
                    last_shown = time(NULL);
                    visible = 1;
                }
            }
            free(event);
        }

        if (visible && time(NULL) - last_shown >= 2) {
            xcb_unmap_window(connection, wid);
            xcb_flush(connection);
            visible = 0;
        }
        usleep(10000);
    }


    xcb_disconnect(connection);
    return 0;
}
