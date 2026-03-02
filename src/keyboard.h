#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "isr.h"
void keyboard_handler(void);
int kbhit(void);
char* kbgets(char* buffer, int max_len);

#endif
