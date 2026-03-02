#ifndef VGA_H
#define VGA_H
#include "../stdint.h"

typedef uint8_t u8;
typedef uint32_t size_t;

typedef struct PointScreen {
    int x;
    int y;
} PointScreen_t;

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define DOUBLE_BUFFER_SIZE VGA_WIDTH * VGA_HEIGHT

#define MISC_PORT   0x3C2
#define SEQ_PORT    0x3C4
#define CRTC_PORT   0x3D4
#define GC_PORT     0x3CE
#define AC_PORT     0x3C0
#define ISR1_PORT   0x3DA
#define DAC_WRITE_INDEX 0x3C8
#define DAC_DATA        0x3C9

void set_vga_mode_320x200x256(void);
void set_palette_color(int index, int r, int g, int b);
void clear_screen(u8 color);
void put_pixel(PointScreen_t point, u8 color);
void draw_line(PointScreen_t a, PointScreen_t b, u8 color);
void draw_rectangular(PointScreen_t p1, PointScreen_t p2, u8 color);
void draw_character(char c, PointScreen_t start_point, int color);
void draw_string(char *str, PointScreen_t start_point, int color);
void render_screen();

#endif
