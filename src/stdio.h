#ifndef STDIO_H1
#define STDIO_H1

void term_init();
void term_putc(char c);
void term_print(const char* str);
void int_to_str(char *buf, int n);
void dbl_to_str(char *buf, double value);
int strcmp(char *str1, char *str2);
int startswith(char *str, char *prefix);
int parseint(char *buf);
double parsenumber(char *buf);
double command_calc(char *str);
void splitstr(char (*out)[10], char *str);
int find_char(char str, char c);

#endif
