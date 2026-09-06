#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "proc_reader.h"
#include "app_env.h"
#include "utils.h"

const unsigned int INITIAL_CAPACITY = 4;

ProcList* create_list()
{
    ProcList *list = (ProcList*) malloc(sizeof(ProcList));
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;

    // aloca so sizeof(*Process) ao inves de alocar o Process.
    // aparentemente sizeof(Process) seria alocacao excessiva.
    //
    // tambem calloc e mais lento que malloc, entao deve ser usado somente na criacao
    // malloc deixa lixo na memoria alocada, entao deve ser usado quando for ser
    // imediatamente preenchido.
    list->items = calloc(list->capacity, sizeof(Process*));

    return list;
}

void push_process(ProcList *list, char *pid)
{
    if(list->count == list->capacity) {
        resize(list);
    }

    Process *p = create_process(pid);
    list->items[list->count++] = p;
}

void resize(ProcList *list)
{
    int new_cap = list->capacity *1.5f;

    Process** new_items = malloc(sizeof(Process*) * new_cap);
    memcpy(new_items, list->items, sizeof(Process*) * list->capacity);

    free(list->items);

    list->capacity = new_cap;
    list->items = new_items;
}

void delete_list(ProcList **list)
{
    free((*list)->items);
    free(*list);
    *list = NULL;
}

// funcao privada desse modulo
// daria pra transformar no get_proc_list, mas achei melhor separar
ProcList* _get_proc_list(char *path) {
    DIR *d;
    d = opendir(path);

    if (!d) {
        perror("Nao foi possivel abrir o diretorio");
        return NULL;
    }

    ProcList *directory = create_list();
    struct dirent *dir;

    while ((dir = readdir(d)) != NULL) {
        // pula diretorios "fake" do linux e os que nao sao processos
        // (.. aponta pro diretorio pai e . aponta pro atual)
        if(!is_numeric(dir->d_name)) {
            continue;
        }

        // precisa alocar em outro lugar, pq dir->d_name possivelmente
        // vai ser destruido sem eu saber.
        char *pid = calloc(strlen(dir->d_name) + 1, sizeof(char));
        strcpy(pid, dir->d_name);
        push_process(directory, pid);
    }

    closedir(d);
    return directory;
}

ProcList* get_proc_list() {
    char *path = PROC_PATH;
    return _get_proc_list(path);
}
