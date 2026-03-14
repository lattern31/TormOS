#include "string.h"

int strcmp(char *s1, char *s2) {
    int i = 0;
    while (s1[i] != '\0' || s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

int startswith(char *s, char *prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if  (s[i] == '\0') {
            return 0;
        }
        if (s[i] != prefix[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

int find(char *s, char c) { 
    //  first entry
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == c) {
            return i;
        }
        i++;
    }
    return -1;
}

void strcpy(char* src, char* dest) {
    char *cpysrc = src;
    while(*cpysrc != '\0') {
        *dest = *cpysrc;
        cpysrc++;
        dest++;
    }
    *dest = '\0';
}

int strlen(char *s) {
  	char *cnt = s;

  	while(*cnt++); // TODO mb not a good idea to let it infinetly increment? 

    return cnt - s - 1;
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

void strrev(char *str) {
    int len = strlen(str);
	int l = 0, r = len - 1;
	while (l < r) {
		char temp = *(str+l);
		*(str+l) = *(str+r);
		*(str+r) = temp;
		l++;
		r--;
	}
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
    
    strrev(buf);
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
