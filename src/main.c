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
    uint32_t value_list[3];
    value_list[0] = 0x000000;
    value_list[1] = BORDER_COLOR;
    value_list[2] = 1;
    
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
                      value_list);


    FT_Context *main_p_fctx = init_freetype();
                      
    uint32_t values[] = { 0xFFFFFF, 0x000000 };
    xcb_gcontext_t gc = xcb_generate_id(connection);
    xcb_create_gc(connection, gc, wid, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND, values);
    
    char text[6];
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
                printf("KEY is RELEASED\n");
                continue;
            }
            else if ((event->response_type & ~0x80) == XCB_KEY_PRESS) {
                printf("KEY is PRESSED\n");
                xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
                if (kp->detail == VOLUME_UP_KEYCODE) {
                    if (visible) {
                        xcb_unmap_window (connection, wid);
                    }
                    xcb_map_window(connection, wid);

                    increase_volume();
                    get_volume ( text, 
                                 size_of_text );
                    render_text ( connection,
                                  wid, 
                                  gc,
                                  screen->root_depth,
                                  main_p_fctx->face,
                                  text );

                    xcb_flush(connection);
                    last_shown = time(NULL);
                    visible = 1;
                }
                else if (kp->detail == VOLUME_DOWN_KEYCODE) {
                    if (visible) {
                        xcb_unmap_window (connection, wid);
                    }
                    xcb_map_window(connection, wid);

                    decrease_volume();
                    get_volume ( text, 
                                 size_of_text );
                    render_text ( connection,
                                  wid, 
                                  gc,
                                  screen->root_depth,
                                  main_p_fctx->face,
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
        usleep(1000);
    }

    FT_Done_Face(main_p_fctx->face);
    FT_Done_FreeType(main_p_fctx->library);
    free(main_p_fctx);

    xcb_disconnect(connection);
    return 0;
}
