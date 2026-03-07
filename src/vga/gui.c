#include "vga.h"
#include "terminal.h"
#include "3d.h"
#import "nya.xpm"

int tab = 0;

int align_center_text(int start, int end, int text_len) {
    // returns start point for text to be aligned by center
    return start + ((end - start - text_len) / 2);
}

void draw_tab(int tab) {
    int border_color = 0;
    int bg_color = 1;
    int select_color = 2;
    int text_color = 2;

    draw_rectangular(
        (PointScreen_t){0, 0},
        (PointScreen_t){VGA_WIDTH, 11},
        bg_color
    );
    draw_line(
        (PointScreen_t){VGA_WIDTH/3, 0},
        (PointScreen_t){VGA_WIDTH/3, 11},
        border_color
    );
    draw_line(
        (PointScreen_t){VGA_WIDTH/3*2, 0},
        (PointScreen_t){VGA_WIDTH/3*2, 11},
        border_color
    );

    draw_string("terminal", (PointScreen_t){
        align_center_text(0, VGA_WIDTH/3*1, 8*8), 2
    }, text_color);
    draw_string("scene0", (PointScreen_t){
        align_center_text(VGA_WIDTH/3*1, VGA_WIDTH/3*2, 8*6), 2
    }, text_color);
    draw_string("scene1", (PointScreen_t){
        align_center_text(VGA_WIDTH/3*2, VGA_WIDTH, 8*6), 2
    }, text_color);
    switch (tab) {
        case 0:
            draw_rectangular(
                (PointScreen_t){0, 0},
                (PointScreen_t){VGA_WIDTH/3, 11},
                select_color
            );
            draw_string("terminal", (PointScreen_t){
                align_center_text(0, VGA_WIDTH/3*1, 8*8), 2
            }, bg_color);
            break;
        case 1:
            draw_rectangular(
                (PointScreen_t){VGA_WIDTH/3, 0},
                (PointScreen_t){VGA_WIDTH/3*2, 11},
                select_color
            );
            draw_string("scene0", (PointScreen_t){
                align_center_text(VGA_WIDTH/3*1, VGA_WIDTH/3*2, 8*6), 2
            }, bg_color);
            break;
        case 2:
            draw_rectangular(
                (PointScreen_t){VGA_WIDTH/3*2, 0},
                (PointScreen_t){VGA_WIDTH, 11},
                select_color
            );
            draw_string("scene1", (PointScreen_t){
                align_center_text(VGA_WIDTH/3*2, VGA_WIDTH, 8*6), 2
            }, bg_color);
            break;
    }
}

void draw_terminal() {
    int start_point_y = 16;
    for (int i = 0; i < (VGA_HEIGHT - 16) / 8; i++) {
        draw_string(
            vga_terminal_history[i],
            (PointScreen_t){0, start_point_y+i*8},
            2
        );
    }
    // draw cursor
    if (vga_terminal_cursor) { 
        draw_character('_',(PointScreen_t){
            vga_terminal_index[1] * 8,
            start_point_y + vga_terminal_index[0] * 8
        }, 2);
    }
    vga_terminal_cursor = !vga_terminal_cursor;
}

void draw_scene(Figure_t *figures[], int quantity) {
    for (int i = 0; i < quantity; i++) {
        draw_figure(figures[i]);
    }
}

void draw_nya_xpm(int8_t scale_size) {
    for (int i = 0; i < 33; i++) {
        for (int j = 0; j < 34; j++) {
            for (int k = 0; k < 12; k++) {
                if (nya_xpm[k+1][0] == nya_xpm[i+13][j]) {
                    for (int q = 0; q < scale_size; q++) {
                        for (int s = 0; s < scale_size; s++) {
                            put_pixel((PointScreen_t){
                                .x=320-(34*scale_size)+j*scale_size+q,
                                .y=200-(33*scale_size)+i*scale_size+s
                            }, k+4);
                        }
                    }
                    break;
                }
            }
        }
    }
}
