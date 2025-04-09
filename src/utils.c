#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void increase_volume() {
    int res = system("wpctl set-volume @DEFAULT_AUDIO_SINK@ 2%+");
    if (res == -1) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%+ \"\n");
    } else if (WIFEXITED(res) && WEXITSTATUS(res) != 0) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%+ \"::Exit_code: %d\n", WEXITSTATUS(res));
    }
}

void decrease_volume() {
    int res = system("wpctl set-volume @DEFAULT_AUDIO_SINK@ 2%-");
    if (res == -1) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%- \"\n");
    } else if (WIFEXITED(res) && WEXITSTATUS(res) != 0) {
        printf("ERROR::Command::\"wpctl set-volume @DEFAULT_AUDIO_SINK 2%%- \"::Exit_code: %d\n", WEXITSTATUS(res));
    }
}


void get_volume(char* text, int size) {
    FILE* fp = popen("pactl get-sink-volume @DEFAULT_SINK@", "r");
    if (!fp) return ;
    int vol = 0;
    fscanf(fp, "%*[^/]/ %d%%", &vol);
    snprintf(text, size, "%d%%", vol);
    pclose(fp);
}

