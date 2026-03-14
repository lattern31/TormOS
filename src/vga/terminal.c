#include "terminal.h"
#include "../stdio.h"

int vga_terminal_cursor = 1;

void vga_terminal_scroll() {}; // TODO
//void vga_terminal_scroll() {
//    for (int i = 1; i < VGA_TERMINAL_HEIGHT; i++) {
//        for (int j = 1; j < VGA_TERMINAL_WIDTH; j++) {
//            vga_terminal_history[i-1][j] = vga_terminal_history[i][j];
//        }
//    }
//    for (int j = 1; j < VGA_TERMINAL_WIDTH; j++) {
//        vga_terminal_history[VGA_TERMINAL_HEIGHT-1][j] = '\0';
//    }
//    vga_terminal_index[0]--;
//}

void vga_terminal_delete(StateTerminal_t *terminal) {
    // TODO deletion for new line
    if (terminal->index[1] > 1 ) {
       terminal->history 
            [terminal->index[0]]
            [terminal->index[1]-1] = '\0';
        terminal->index[1]--;
        terminal->cmd_ptr--;
        *terminal->cmd_ptr = '\0';
    }
};

void vga_terminal_print(StateTerminal_t *terminal, char *s) {
    //  terminal got printed actually in draw_terminal XD
    char *str = s;
    while (*str) {
        if (*str == '\n') {
            terminal->index[0]++;
            terminal->index[1] = 0;
            if (terminal->index[0] >= VGA_TERMINAL_HEIGHT) {
                vga_terminal_scroll();
            }
            str++;
            continue;
        }
        if (terminal->index[1] >= VGA_TERMINAL_WIDTH) {
            terminal->index[0]++;
            terminal->index[1] = 0;
        } 
        if (terminal->index[0] >= VGA_TERMINAL_HEIGHT) {
            vga_terminal_scroll();
        }
        terminal->history
            [terminal->index[0]]
            [terminal->index[1]] = *str;

        terminal->index[1]++;
        str++;

    }
};
