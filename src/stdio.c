#include "stdint.h"
#define VGA_COLS 80
#define VGA_ROWS 25

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

int term_col = 0;
int term_row = 0;
uint8_t term_color = 0x0F; // Black background, White foreground

char term_history[VGA_ROWS][VGA_COLS];

void term_init() {
	for (int col = 0; col < VGA_COLS; col ++) {
		for (int row = 0; row < VGA_ROWS; row ++) {
			const size_t index = (VGA_COLS * row) + col;
			// Entries in the VGA buffer take the binary form BBBBFFFFCCCCCCCC, where:
			// - B is the background color
			// - F is the foreground color
			// - C is the ASCII character
			vga_buffer[index] = ((uint16_t)term_color << 8) | ' '; 
		}
	}
}

void term_putc(char c) {
	switch (c) {
		case '\n': 
		{
			term_col = 0;
			term_row++;
			break;
		}

		default: 
		{
            term_history[term_row][term_col] = c;
			const size_t index = (VGA_COLS * term_row) + term_col; 
			vga_buffer[index] = ((uint16_t)term_color << 8) | c;
			term_col ++;
			break;
		}
	}


	if (term_col >= VGA_COLS) {
		term_col = 0;
		term_row ++;
	}

	if (term_row >= VGA_ROWS) {
        for (int i = 1; i < VGA_ROWS; i++) {
            for (int j = 0; j < VGA_COLS; j++) {
                char history_char;
                if (term_history[i][j] == '\0') {
                    history_char = ' ';
                } else {
                    history_char = term_history[i][j];
                }
                term_history[i-1][j] = history_char;

                const size_t history_index = (i-1) * VGA_COLS + j;
                vga_buffer[history_index] = ((uint16_t)term_color << 8) | history_char;
            }
        }
        for (int i = 0; i < VGA_COLS; i++) {
            term_history[VGA_ROWS-1][i] = ' ';
            vga_buffer[(VGA_ROWS-1) * VGA_COLS + i] = ((uint16_t)term_color << 8) | ' ';
        }
		term_col = 0;
		term_row = term_row - 1;
	}
}

void term_print(const char* str) {
	for (size_t i = 0; str[i] != '\0'; i ++) 
		term_putc(str[i]);
}
