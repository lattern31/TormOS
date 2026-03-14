#include "nparse.h"

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
    while (buf[i] == '-') {
        negative_multiplier *= -1;
        i++;
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
