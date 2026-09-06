// define o ambiente da aplicacao, ou seja, se esta rodando teste ou o main.c
// isso faz diferenca, pois no main.c sera usado o /proc e nos testes, o fixture.
#ifndef APP_ENV_H
#define APP_ENV_H

#ifndef IS_TESTING
#define IS_TESTING 0
#endif

#if IS_TESTING
    #define PROC_PATH "./tests/fixtures/proc"
#else
    #define PROC_PATH "/proc"
#endif

#endif
