// define o ambiente da aplicacao, ou seja, se esta rodando teste ou o main.c
// isso faz diferenca, pois no main.c sera usado o /proc e nos testes, o fixture.
#ifndef APP_PROC_TABLE_H
#define APP_PROC_TABLE_H
#include <pthread.h>
#include "proc_reader.h"

typedef struct
{
    ProcList *procs_before;
    ProcList *procs_after;
    unsigned long int cpu_before;
    unsigned long int cpu_after;
    int dados_prontos;
    pthread_mutex_t mutex; // Protege o acesso aos ponteiros dinâmicos
} TableInfo;

void *update_table_info();
void update_proc_values(ProcList **procs_before, ProcList **procs_after, unsigned long int *cpu_before, unsigned long int *cpu_after);
void print_proc_table(
    ProcList *procs_before,
    ProcList *procs_after,
    unsigned long int cpu_before,
    unsigned long int cpu_after,
    unsigned long int memory_total,
    int selected_index);

int getch_timeout(void);

#endif
