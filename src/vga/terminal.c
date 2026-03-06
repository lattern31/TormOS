#include "terminal.h"

char vga_terminal_history[VGA_TERMINAL_HEIGHT][VGA_TERMINAL_WIDTH];
char vga_terminal_command[256];
char *vga_terminal_command_pointer = vga_terminal_command;
int vga_terminal_index[2] = {0, 1};
int vga_terminal_cursor = 1;

void vga_terminal_scroll() {
    for (int i = 1; i < VGA_TERMINAL_HEIGHT; i++) {
        for (int j = 1; j < VGA_TERMINAL_WIDTH; j++) {
            vga_terminal_history[i-1][j] = vga_terminal_history[i][j];
        }
    }
    for (int j = 1; j < VGA_TERMINAL_WIDTH; j++) {
        vga_terminal_history[VGA_TERMINAL_HEIGHT-1][j] = '\0';
    }
    vga_terminal_index[0]--;
}

void vga_terminal_print(char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == '\n') {
            vga_terminal_index[0]++;
            vga_terminal_index[1] = 0;
            if (vga_terminal_index[0] >= VGA_TERMINAL_HEIGHT) {
                vga_terminal_scroll();
            }
            i++;
            continue;
        }
        if (vga_terminal_index[1] >= VGA_TERMINAL_WIDTH) {
            vga_terminal_index[0]++;
            vga_terminal_index[1] = 0;
        } 
        if (vga_terminal_index[0] >= VGA_TERMINAL_HEIGHT) {
            vga_terminal_scroll();
        }
        vga_terminal_history
            [vga_terminal_index[0]]
            [vga_terminal_index[1]] = s[i];
        vga_terminal_index[1]++;
        i++;
    }
}
