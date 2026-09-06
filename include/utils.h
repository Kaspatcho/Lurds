#ifndef UTILS_H
#define UTILS_H

char* join_path(char *basepath, char *path);
int startswith(const char *str, const char *prefix);
char* str_slice(char *string, size_t len);
char* str_slice_until(char *string, size_t start, const char c);
int is_numeric(char *str);

#endif
