// contem get_proc_list que le o /proc e cria a lista de processos.
#ifndef PROC_READER_H
#define PROC_READER_H
#include "proc_parser.h"

typedef struct ProcList {
    size_t count;
    size_t capacity;
    Process** items; // array de processos
} ProcList;

// cria ProcList vazia
ProcList* create_list();

// cria Process com base no PID (com create_process)
// e adiciona essa struct na lista.
// se redimensiona automaticamente (chama resize)
void push_process(ProcList *list, char *pid);

// redimensiona a lista criando um novo array maior
// e copiando os dados do antigo para o novo
void resize(ProcList *list);

// destroi a lista e libera memoria alocada
void delete_list(ProcList **list);

// le /proc e cria lista de acordo com ele.
ProcList* get_proc_list();

#endif
