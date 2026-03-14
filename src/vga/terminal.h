#ifndef VGA_TERMINAL_H
#define VGA_TERMINAL_H

#include "vga.h"

#define VGA_TERMINAL_HEIGHT VGA_HEIGHT / 8 - 2
#define VGA_TERMINAL_WIDTH VGA_WIDTH / 8

typedef struct StateTerminal {
    char history[VGA_TERMINAL_HEIGHT][VGA_TERMINAL_WIDTH];
    char command[128];
    uint8_t index[2];
    char *cmd_ptr;
} StateTerminal_t;

extern char vga_terminal_history[VGA_TERMINAL_HEIGHT][VGA_TERMINAL_WIDTH];
extern char vga_terminal_command[256];
extern char *vga_terminal_command_pointer;
extern int vga_terminal_index[2];
extern int vga_terminal_cursor;

void vga_terminal_scroll();
//void vga_terminal_print(char s[]);
void vga_terminal_print(StateTerminal_t *terminal, char *s);
void vga_terminal_delete(StateTerminal_t *terminal);

#endif
