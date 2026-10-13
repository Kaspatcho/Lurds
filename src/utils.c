#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

char* join_path(char *basepath, char *path)
{
    size_t length = 40;
    char *newpath = calloc(length, sizeof(char));
    snprintf(newpath, length, "%s/%s", basepath, path);

    return newpath;
}

int startswith(const char *str, const char *prefix) {
    size_t len_prefix = strlen(prefix);
    size_t len_str = strlen(str);

    if (len_prefix > len_str) {
        return 0;
    }

    return strncmp(str, prefix, len_prefix) == 0;
}

char* str_slice(char *string, size_t len)
{
    char *n = calloc(strlen(string) - len + 1, sizeof(char));
    strcpy(n, ((char*) string) + len);
    return n;
}

char* str_slice_until(char *string, size_t start, const char c)
{
    size_t length = 0;
 
    while (string[start + length] != '\0' &&
           string[start + length] != c) {
        length++;
    }
 
    char *n = calloc(length + 1, sizeof(char));
    if (n == NULL) {
        return NULL;
    }
 
    strncpy(n, string + start, length);
    n[length] = '\0';
 
    return n;
}

int is_numeric(char *str)
{
    int i = 0;
    while(str[i] != '\0') {
        if(!isdigit(str[i++])) return 0;
    }

    return 1;
}
