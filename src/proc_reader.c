#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "proc_reader.h"
#include "flags.h"

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
    char *path = TESTING ? "./tests/fixtures/proc" : "/proc";
    return _get_proc_list(path);
}

// - read status
// - read stat
// - read cmdline
// char* read_files(char *pid)
// {
//     FILE *fixture;
//     char content[64] = {0};
//     size_t bytes_read;

//     fixture = fopen("tests/fixtures/example.txt", "r");
//     assert_non_null(fixture);

//     bytes_read = fread(content, 1, sizeof(content) - 1, fixture);
//     assert_int_equal(fclose(fixture), 0);
//     assert_int_equal(bytes_read, 25);
//     assert_string_equal(content, "Fixture example content.\n");
// }