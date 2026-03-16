#include "gui.h"
#include "terminal.h"
#include "3d.h"
#include "nya.xpm"
#include "../common.h"
#include "../stdio.h"
#include "../string.h"
#include "../heap.h"

int tab = 0;
Tab_t *tab_stack[MAXTAB+1];

void draw_intro() {
    clear_screen(15);
    draw_nya_xpm(5);
    draw_string("TormOS", (PointScreen_t){50, 100}, 5);
}

int align_center_text(int start, int end, int text_width) {
    // returns start point for text to be aligned by center
    // note text_width in pixels
    return start + ((end - start - text_width) / 2);
}

Tab_t* create_tab(TabType type) {
    Tab_t *tab = (Tab_t*)malloc(sizeof(Tab_t));
    tab->type = type;

    if (type == TERMINAL) {
        for (int i = 0; i < VGA_TERMINAL_HEIGHT; i++) {
            for (int j = 0; j < VGA_TERMINAL_WIDTH; j++) {
                tab->state.terminal.history[i][j] = '\0';
            }
        }
        for (int i = 0; i < 128; i++) {
            tab->state.terminal.command[i] = '\0';
        }
        tab->state.terminal.history[0][0] = '$';
        tab->state.terminal.index[0] = 0;
        tab->state.terminal.index[1] = 1;
        tab->state.terminal.cmd_ptr = &tab->state.terminal.command[0];
    } else if (type == SCENE) {
        //  nothing here?
    } else if (type == INFO) {
        //  nothing here?
    }
    return tab;
}

void draw_navbar() {
    int border_color = 0;
    int bg_color = 1;
    int text_color = 2;
    int selected_bg_color = 2;
    int selected_text_color = 1;

    int tabcnt = 0;
    while (tab_stack[tabcnt] != INVALID_PTR) {
        tabcnt++;
    }

    draw_rectangular(
        (PointScreen_t){0, 0},
        (PointScreen_t){VGA_WIDTH, 11},
        bg_color
    );
    for (int i = 0; i < tabcnt-1; i++) { // separate vertical lines
        for (int q = 0; q <= 11; q++) {
            put_pixel((PointScreen_t){VGA_WIDTH/tabcnt*(i+1), q}, border_color);
        }
    }
    char tabname[16];
    int cur_text_color;
    for (int i = 0; i < tabcnt; i++) {
        switch (tab_stack[i]->type) {
            case TERMINAL:
                strcpy("TERMINAL", tabname);
                break;
            case SCENE:
                strcpy("SCENE", tabname);
                break;
            case INFO:
                strcpy("INFO", tabname);
                break;
        }

        if (i == tab) { //  selected
            draw_rectangular(
                (PointScreen_t){VGA_WIDTH/tabcnt*(i), 0},
                (PointScreen_t){VGA_WIDTH/tabcnt*(i+1), 11},
                selected_bg_color
            );
            cur_text_color = selected_text_color;
        } else {
            cur_text_color = text_color;
        }
        draw_string(
            tabname,
            (PointScreen_t){
                align_center_text(
                    VGA_WIDTH/tabcnt*(i),
                    VGA_WIDTH/tabcnt*(i+1),
                    strlen(tabname) * 8
                ),
                2
            },
            cur_text_color
        );
    }
}

void draw_terminal(StateTerminal_t *terminal) {
    int start_point_y = 16;
    for (int i = 0; i < VGA_TERMINAL_HEIGHT; i++) {
        draw_string(
            terminal->history[i],
            (PointScreen_t){0, start_point_y+i*8},
            2
        );
    }
    // draw cursor
    if (vga_terminal_cursor) { 
        draw_character('_',(PointScreen_t){
            terminal->index[1] * 8,
            start_point_y + terminal->index[0] * 8
        }, 2);
    }
    static int blink_counter = 0;
    blink_counter++;
    if (blink_counter >= 30) {
        vga_terminal_cursor = !vga_terminal_cursor;
        blink_counter = 0;
    }
}

void draw_scene(Figure_t *figures[], int quantity) {
    for (int i = 0; i < quantity; i++) {
        draw_figure(figures[i]);
    }
}

void draw_info() {
    char row[32];

    // tab count
    int cnt = 0;
    while (tab_stack[cnt] != INVALID_PTR) {
        cnt++;
    }
    strcpy("tabs = ", row);
    int_to_str(row+strlen(row), cnt);
    draw_string(row, (PointScreen_t){.x=10, .y=20}, 2);

    // heap_start 
    extern uint32_t heap_start;
    strcpy("heap start = ", row);
    int_to_str(row+strlen(row), (uint32_t)&heap_start);
    draw_string(row, (PointScreen_t){.x=10, .y=30}, 3);

    // heap usage
    uint32_t heap_usage = (uint32_t)kernel_heap.fblock + kernel_heap.fblock->size - (uint32_t)&heap_start;
    strcpy("heap usage = ", row);
    int_to_str(row+strlen(row), heap_usage);
    draw_string(row, (PointScreen_t){.x=10, .y=40}, 3);

    // stack free
    extern uint32_t stack_top;
    uint32_t stack_used = 0;
    asm volatile("mov %%esp, %0" : "=r"(stack_used));
    uint32_t stack_free = (uint32_t)&stack_top - stack_used;
    strcpy("stack free = ", row);
    int_to_str(row+strlen(row), stack_free);
    draw_string(row, (PointScreen_t){.x=10, .y=50}, 2);

    uint32_t fblock_size = (uint32_t)kernel_heap.fblock->size;
    strcpy("fblock size = ", row);
    int_to_str(row+strlen(row), fblock_size);
    draw_string(row, (PointScreen_t){.x=10, .y=60}, 3);

    uint32_t fblock_rel_start = (uint32_t)kernel_heap.fblock - (uint32_t)&heap_start;
    strcpy("fblock relative start = ", row);
    int_to_str(row+strlen(row), fblock_rel_start);
    draw_string(row, (PointScreen_t){.x=10, .y=70}, 3);
}

void draw_nya_xpm(uint8_t scale_size) {
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
