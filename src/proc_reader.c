#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "proc_reader.h"
#include "app_env.h"

ProcList* _make_list(char *pid) {
    ProcList *dir = (ProcList*) malloc(sizeof(ProcList));
    dir->pid = pid;

    return dir;
}

ProcList *_add_proc(ProcList* list, char *name) {
    ProcList* node = _make_list(name);
    node->next = list;

    return node;
}

ProcList* _get_proc_list(char *path) {
    DIR *d;
    d = opendir(path);

    if (!d) {
        perror("Nao foi possivel abrir o diretorio");
        return NULL;
    }

    ProcList *directory = _make_list(NULL);
    struct dirent *dir;

    while ((dir = readdir(d)) != NULL) {
        directory = _add_proc(directory, dir->d_name);
    }

    closedir(d);
    return directory;
}

ProcList* get_proc_list() {
    char *path = PROC_PATH;
    return _get_proc_list(path);
}
