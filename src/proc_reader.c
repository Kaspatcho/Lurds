#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "proc_reader.h"
#include "app_env.h"
#include "utils.h"

const unsigned int INITIAL_CAPACITY = 4;

ProcList *create_list()
{
    ProcList *list = (ProcList *)malloc(sizeof(ProcList));
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;

    // aloca so sizeof(*Process) ao inves de alocar o Process.
    // aparentemente sizeof(Process) seria alocacao excessiva.
    //
    // tambem calloc e mais lento que malloc, entao deve ser usado somente na criacao
    // malloc deixa lixo na memoria alocada, entao deve ser usado quando for ser
    // imediatamente preenchido.
    list->items = calloc(list->capacity, sizeof(Process *));

    return list;
}

void push_process(ProcList *list, char *pid)
{
    if (list->count == list->capacity)
    {
        resize(list);
    }

    Process *p = create_process(pid);

    if (p == NULL)
    {
        return;
    }

    list->items[list->count++] = p;
}

void resize(ProcList *list)
{
    int new_cap = list->capacity * 1.5f;

    Process **new_items = malloc(sizeof(Process *) * new_cap);
    memcpy(new_items, list->items, sizeof(Process *) * list->capacity);

    free(list->items);

    list->capacity = new_cap;
    list->items = new_items;
}

void delete_list(ProcList **list)
{
    if (list == NULL || *list == NULL)
    {
        return;
    }

    for (size_t i = 0; i < (*list)->count; i++)
    {
        Process *p = (*list)->items[i];

        if (p != NULL)
        {
            free(p->pid);
            free(p->name);
            free(p->ppid);
            free(p->uid);
            free(p->state);
            free(p->vmrss);
            free(p);
        }
    }

    free((*list)->items);
    free(*list);
    *list = NULL;
}

// funcao privada desse modulo
// daria pra transformar no get_proc_list, mas achei melhor separar
ProcList *_get_proc_list(char *path)
{
    DIR *d = opendir(path);

    if (!d)
    {
        perror("Nao foi possivel abrir o diretorio");
        return NULL;
    }

    ProcList *directory = create_list();
    struct dirent *dir;

    while ((dir = readdir(d)) != NULL)
    {
        // Ignora entradas que nao representam PIDs numericos
        if (!is_numeric(dir->d_name))
        {
            continue;
        }
        push_process(directory, dir->d_name);
    }

    closedir(d);
    return directory;
}

ProcList *get_proc_list()
{
    char *path = PROC_PATH;
    return _get_proc_list(path);
}
