#ifndef COWDOOM_VIDEO_H
#define COWDOOM_VIDEO_H

#include <stdbool.h>

bool video_init(int cols, int rows, int char_width, int char_height, const char *title);
void video_shutdown(void);
void video_toggle_fullscreen(void);
void video_clear(void);
void video_draw_ascii(const char *buffer, int cols, int rows);
void video_draw_border_flash(float alpha);
void video_present(void);
int  video_consume_mouse_dx(void);

#endif
