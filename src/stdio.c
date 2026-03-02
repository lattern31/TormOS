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

void rev_str(char *str, long int len) {
	int l = 0, r = len - 1;
	while (l < r) {
		char temp = *(str+l);
		*(str+l) = *(str+r);
		*(str+r) = temp;
		l++;
		r--;
	}
}
int findLen(char *s) {
  	char* last = s;

  	while(*last++);

    return last - s - 1;
}
void int_to_str(char *buf, int n) {
    const int ASCII_START_NUMBERS = 48;
    int is_negative = 0;
    
    if (n == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    int i = 0;

    if (n < 0) {
        is_negative = 1;
        n *= -1;
    }
    
    while (n > 0) {
        buf[i] = (char)(ASCII_START_NUMBERS + (n % 10));
        n = n / 10;
        i++;
    }
    if (is_negative) {
        buf[i++] = '-';
    }
    buf[i] = '\0';
    
    rev_str(buf, i);
}

void dbl_to_str(char *buf, double value) {
    int i = 0;
    long integer = (long)value;
    double fraction = value - integer;
    if (integer < 0 || fraction < 0) {
        integer = integer < 0 ? -integer : integer;
        fraction = fraction < 0 ? -fraction: fraction;
        buf[0] = '-';
        i++;
    }
    int_to_str(buf+i, integer);
    if (fraction == 0) return;
    while (buf[i] != '\0') i++;
    buf[i++] = '.';
    int_to_str(buf+i, (long)(fraction * 100.0));
}

int char_to_int(char c) {
    const int ASCII_START_NUMBERS = 48;
    int digit = c - ASCII_START_NUMBERS;
    if (digit > 9 || digit < 0) {
        return -1;
    }
    return digit;
}
double parsenumber(char *buf) {
    int i = 0;
    int negative_multiplier = 1;
    if (buf[0] == '-') {
        i = 1; // start from second char if negative
        negative_multiplier = -1;
    }

    int is_fractional_part = 0;
    int n = 0;
    while (buf[i] != '\0') { // integer part
        if (buf[i] == '.') {
            break;
        }
        int digit = char_to_int(buf[i]);
        n = n * 10 + digit;
        i++;
    }
    double fractional_part = 0;
    int fractional_multiplier = 10;
    int fractional_index = 10;
    while (is_fractional_part && buf[i] != '\0') { // fractional part
        int digit = char_to_int(buf[i]);
        fractional_part += 1 / fractional_index * digit;
        fractional_multiplier *= 10;
        fractional_index++;
        i++;
    }
    n += fractional_part;
    n *= negative_multiplier;

    return n;
}
int strcmp(char *str1, char *str2) {
    int i = 0;
    while (str1[i] != '\0' || str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}
int startswith(char *str, char *prefix)
{
    int i = 0;
    while (prefix[i] != '\0') {
        if  (str[i] == '\0') {
            return 0;
        }
        if (str[i] != prefix[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}
void splitstr(char (*out)[10], char *str) {
    int i = 0;
    int argindex = 0;
    int strindex = 0;
    while(str[i] != '\0') {
       if (str[i] == ' ') {
           out[argindex][strindex] = '\0';
           argindex++;
           strindex=0;
           i++;
           continue;
       }
       out[argindex][strindex] = str[i];
       strindex++;
       i++;
    }
   out[argindex][strindex] = '\0';
}
double calc(double a, double b, char op[2]) {
    switch (*op) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        default:
            term_print("invalid op\n");
            return 0;
    }
}
double command_calc(char *str) { // TODO return double 
    char args[5][10];
    splitstr(args, str);

    double a = parsenumber(args[1]);
    if (a == -1) {
        term_print("invalid first arg");
        return 0;
    }

    double b = parsenumber(args[3]);
    if (b == -1) {
        term_print("invalid second arg");
        return 0;
    }
    return calc(a, b, args[2]);
}
