#include <stddef.h>
#include "string.h"

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++)) ;
    return dest;
}

char *strtok(char *str, const char *delim) {
    static char *last;
    char *token_start;
    
    if (str) last = str;
    if (!last) return NULL;
    
    // Пропускаем разделители
    while (*last && strchr(delim, *last)) last++;
    if (*last == '\0') return NULL;
    
    token_start = last;
    
    // Ищем конец токена
    while (*last && !strchr(delim, *last)) last++;
    
    if (*last) {
        *last = '\0';
        last++;
    }
    
    return token_start;
}

char *strchr(const char *s, int c) {
    while (*s && *s != (char)c) s++;
    return (*s == (char)c) ? (char*)s : NULL;
}