#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"
#include "enums.h"
#include "config.h"

#include <ft2build.h>
#include FT_FREETYPE_H


void pre_load_bitmaps ( bitmap_data**  bitmap_holder,
                        FT_Face     face ) {

    for (int i=0; i<=9; i++) {
        if (FT_Load_Char(face, i+'0', FT_LOAD_RENDER)) {
            printf("ERROR::utils::FT_Load_Char()\n");
        }
        FT_Bitmap* bitmap = &face->glyph->bitmap;

        bitmap_holder[i] = malloc(sizeof(bitmap_data));
    
        bitmap_holder[i]->rows = bitmap->rows;
        bitmap_holder[i]->width = bitmap->width;
        bitmap_holder[i]->data = malloc(bitmap->rows * bitmap->width * sizeof(uint32_t));
        
        for (unsigned int y = 0; y < bitmap->rows; y++) {
            for (unsigned int x = 0; x < bitmap->width; x++) {
                bitmap_holder[i]->data[y * bitmap->width + x] = bitmap->buffer[y * bitmap->width + x] ? 1 : 0;
            }
        }
        /*printf("Loading %d done\n", i);*/
    }
}

void increase_volume() {
    int res = system("wpctl set-volume @DEFAULT_AUDIO_SINK@ 1%+");
    if (res == -1) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%+ \"\n");
    } else if (WIFEXITED(res) && WEXITSTATUS(res) != 0) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%+ \"::Exit_code: %d\n", WEXITSTATUS(res));
    }
}

void decrease_volume() {
    int res = system("wpctl set-volume @DEFAULT_AUDIO_SINK@ 1%-");
    if (res == -1) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%- \"\n");
    } else if (WIFEXITED(res) && WEXITSTATUS(res) != 0) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%- \"::Exit_code: %d\n", WEXITSTATUS(res));
    }
}

// TODO :: remove this function and somehow use libpulse to fetch volume
void get_volume(char* text, int size) {
    FILE* fp = popen("pactl get-sink-volume @DEFAULT_SINK@", "r");
    if (!fp) return ;
    int vol = 0;
    fscanf(fp, "%*[^/]/ %d%%", &vol);
    snprintf(text, size, "%d", vol);
    pclose(fp);
}
