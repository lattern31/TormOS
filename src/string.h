#ifndef STRING_H
#define STRING_H

void int_to_str(char *buf, int n);
void dbl_to_str(char *buf, double value);
int strcmp(char *s1, char *s2);
int startswith(char *s, char *prefix);
void splitstr(char (*out)[10], char *s);
int find(char *s, char c);
void strcpy(char* src, char* dest);
void strrev(char *s);
int strlen(char *s);

#endif
