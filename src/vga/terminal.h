#ifndef VGA_TERMINAL_H
#define VGA_TERMINAL_H

#include "vga.h"

#define VGA_TERMINAL_HEIGHT (VGA_HEIGHT - 16) / 8
#define VGA_TERMINAL_WIDTH VGA_WIDTH / 8

extern char vga_terminal_history[VGA_TERMINAL_HEIGHT][VGA_TERMINAL_WIDTH];
extern char vga_terminal_command[256];
extern char *vga_terminal_command_pointer;
extern int vga_terminal_index[2];
extern int vga_terminal_cursor;

void vga_terminal_scroll();
void vga_terminal_print(char s[]);

#endif
