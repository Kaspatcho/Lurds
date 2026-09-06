#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proc_parser.h"
#include "utils.h"
#include "app_env.h"

/*
# proc_parser
- read proc files (status, stat and cmdline for now)
- turn proc files into a Process Struct
- make dynamic vector of Process called ProcList
*/

Process* create_process(char *pid)
{
    Process *p = (Process*) malloc(sizeof(Process));
    p->pid = pid;

    read_status(p);
    read_cmdline(p);
    return p;
}

int read_status(Process *p)
{
    char *basepath = join_path(PROC_PATH, p->pid);
    char *status = join_path(basepath, "status");

    FILE *file = fopen(status, "r");

    if (file == NULL) {
        perror("Erro ao abrir arquivo de status");
        return 1;
    }

    char line[256];


    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        // codigo SUPER porco, mas que funciona
        if (startswith(line, "State:")) {
            // 7 = tamanho de "State:\t"
            p->state = str_slice(line, 7);
            continue;
        }

        if (startswith(line, "PPid:")) {
            p->ppid = str_slice(line, 6);
            continue;
        }

        if (startswith(line, "Uid:")) {
            p->uid = str_slice_until(line, 5, '\t');
            continue;
        }

        if (startswith(line, "VmRSS:")) {
            p->vmrss = str_slice(line, 11);
            break; // ultimo. nao precisa mais ler o arquivo
        }
    }

    // 3. Always close the file when done
    fclose(file);
    return 0;
}

int read_cmdline(Process *p)
{
    char *basepath = join_path(PROC_PATH, p->pid);
    char *status = join_path(basepath, "cmdline");

    FILE *file = fopen(status, "r");

    if (file == NULL) {
        perror("Erro ao abrir arquivo de status");
        return 1;
    }

    char command[256] = {0};

    if(fgets(command, sizeof(command), file) == NULL) {
        perror("Erro! /cmdline devia ter uma linha.\n");
        fclose(file);
        return 1;
    }

    int first = 0;
    while(command[++first] != '\0');

    int last = 255;
    while(command[--last] == '\0');

    for(int i = first; i <= last; i++) {
        if(command[i] == '\0') command[i] = ' ';
    }

    command[last + 1] = '\0';

    p->name = calloc(last, sizeof(char));
    strncpy(p->name, command, last + 1);

    fclose(file);
    return 0;
}

// - read stat (TODO)
// trocar cpu_percentage bruto por delta total e delta idle pra calcular no futuro.
