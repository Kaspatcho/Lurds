// modulo com funcoes que podem ser uteis, ja que o C e do tempo das cavernas
#ifndef UTILS_H
#define UTILS_H

// concatena um path base com um sufixo
// ex. "./tests" + "example.txt" deve retornar "./tests/example.txt"
char* join_path(char *basepath, char *path);

// verifica se uma string str comeca com o prefixo prefix
int startswith(const char *str, const char *prefix);

// retorna uma fatia da string original
char* str_slice(char *string, size_t len);

// mesma coisa que str_lice, mas vai ate um charartere c especifico.
char* str_slice_until(char *string, size_t start, const char c);

// verifica se a string str e totalmente numerica.
int is_numeric(char *str);

#endif
