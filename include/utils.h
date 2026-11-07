// modulo com funcoes que podem ser uteis, ja que o C e do tempo das cavernas
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// concatena um path base com um sufixo
// ex. "./tests" + "example.txt" deve retornar "./tests/example.txt"
char *join_path(const char *basepath, const char *path);

// verifica se uma string str comeca com o prefixo prefix
int startswith(const char *str, const char *prefix);

// retorna uma fatia da string original
char *str_slice(const char *string, size_t len);

// mesma coisa que str_lice, mas vai ate um charartere c especifico.
char *str_slice_until(const char *string, size_t start, char c);

// verifica se a string str e totalmente numerica.
int is_numeric(const char *str);

#endif
