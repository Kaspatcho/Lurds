#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "utils.h"

char *join_path(const char *basepath, const char *path)
{
    if (basepath == NULL || path == NULL)
    {
        return NULL;
    }

    size_t base_length = strlen(basepath);
    size_t path_length = strlen(path);

    if (base_length > SIZE_MAX - path_length - 2)
    {
        return NULL;
    }

    size_t length = base_length + path_length + 2;
    char *newpath = malloc(length);
    if (newpath == NULL)
    {
        return NULL;
    }

    snprintf(newpath, length, "%s/%s", basepath, path);

    return newpath;
}

int startswith(const char *str, const char *prefix)
{
    if (str == NULL || prefix == NULL)
    {
        return 0;
    }

    size_t len_prefix = strlen(prefix);
    size_t len_str = strlen(str);

    if (len_prefix > len_str)
    {
        return 0;
    }

    return strncmp(str, prefix, len_prefix) == 0;
}

char *str_slice(const char *string, size_t len)
{
    if (string == NULL)
    {
        return NULL;
    }

    size_t string_length = strlen(string);
    if (len > string_length)
    {
        return NULL;
    }

    char *n = malloc(string_length - len + 1);
    if (n == NULL)
    {
        return NULL;
    }

    memcpy(n, string + len, string_length - len + 1);
    return n;
}

char *str_slice_until(const char *string, size_t start, char c)
{
    if (string == NULL || start > strlen(string))
    {
        return NULL;
    }

    size_t length = 0;

    while (string[start + length] != '\0' &&
           string[start + length] != c)
    {
        length++;
    }

    char *n = calloc(length + 1, sizeof(char));
    if (n == NULL)
    {
        return NULL;
    }

    strncpy(n, string + start, length);
    n[length] = '\0';

    return n;
}

int is_numeric(const char *str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }

    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)str[i]))
        {
            return 0;
        }
    }

    return 1;
}
