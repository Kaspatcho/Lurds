#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>

#include "app_env.h"
#include "proc_reader.h"
#include "utils.h"

static const size_t INITIAL_CAPACITY = 4;

ProcList *create_list(void)
{
    ProcList *list = malloc(sizeof(*list));
    if (list == NULL)
    {
        return NULL;
    }

    list->items = calloc(INITIAL_CAPACITY, sizeof(*list->items));
    if (list->items == NULL)
    {
        free(list);
        return NULL;
    }

    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    return list;
}

void resize(ProcList *list)
{
    if (list == NULL || list->capacity == 0 ||
        list->capacity > SIZE_MAX - list->capacity / 2 - 1)
    {
        return;
    }

    size_t new_capacity = list->capacity + list->capacity / 2;
    if (new_capacity == list->capacity || new_capacity > SIZE_MAX / sizeof(*list->items))
    {
        return;
    }

    Process **items = realloc(list->items, new_capacity * sizeof(*items));
    if (items == NULL)
    {
        return;
    }

    list->items = items;
    list->capacity = new_capacity;
}

void push_process(ProcList *list, const char *pid)
{
    if (list == NULL || list->items == NULL || !is_numeric(pid))
    {
        return;
    }

    if (list->count == list->capacity)
    {
        resize(list);
    }
    if (list->count == list->capacity)
    {
        return;
    }

    Process *process = create_process(pid);
    if (process != NULL)
    {
        list->items[list->count++] = process;
    }
}

void delete_list(ProcList **list)
{
    if (list == NULL || *list == NULL)
    {
        return;
    }

    for (size_t i = 0; i < (*list)->count; i++)
    {
        delete_process(&(*list)->items[i]);
    }

    free((*list)->items);
    free(*list);
    *list = NULL;
}

static ProcList *get_proc_list_from_path(const char *path)
{
    DIR *directory_stream = opendir(path);
    if (directory_stream == NULL)
    {
        return NULL;
    }

    ProcList *directory = create_list();
    if (directory == NULL)
    {
        closedir(directory_stream);
        return NULL;
    }

    struct dirent *entry;
    while ((entry = readdir(directory_stream)) != NULL)
    {
        if (is_numeric(entry->d_name))
        {
            push_process(directory, entry->d_name);
        }
    }

    closedir(directory_stream);
    return directory;
}

ProcList *get_proc_list(void)
{
    return get_proc_list_from_path(PROC_PATH);
}
