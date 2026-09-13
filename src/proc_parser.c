#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proc_parser.h"
#include "utils.h"
#include "app_env.h"

Process* create_process(char *pid)
{
    Process *p = (Process*) malloc(sizeof(Process));
    p->pid = pid;
    p->name = NULL;
    p->ppid = NULL;
    p->uid = NULL;
    p->state = NULL;
    p->vmrss = NULL;
    p->active_time = 0;
    p->start_time = 0;

    read_status(p);
    int error = read_cmdline(p);
    if(error) {
        read_comm(p);
    }

    read_stat(p);
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

    int has_vmrss = 0;

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
            has_vmrss = 1;
            break; // ultimo. nao precisa mais ler o arquivo
        }
    }

    // adiciona valor padrao caso nao seja encontrado.
    if(!has_vmrss) {
        p->vmrss = "0 kB";
    }

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

    p->name = calloc(last + 1, sizeof(char));
    strncpy(p->name, command, last + 1);

    fclose(file);
    return 0;
}

int read_comm(Process *p)
{
    char *basepath = join_path(PROC_PATH, p->pid);
    char *status = join_path(basepath, "comm");

    FILE *file = fopen(status, "r");

    if (file == NULL) {
        perror("Erro ao abrir arquivo de status");
        return 1;
    }

    char command[256] = {0};

    if(fgets(command, sizeof(command), file) == NULL) {
        perror("Erro! /comm devia ter uma linha.\n");
        fclose(file);
        return 1;
    }

    int last = 0;
    char c;
    while((c = command[++last]) != '\n' && c != '\0');

    p->name = calloc(last + 1, sizeof(char));
    strncpy(p->name, command, last + 1);
    p->name[last] = '\0';

    fclose(file);
    return 0;
}

int read_stat(Process *p)
{
    char *basepath = join_path(PROC_PATH, p->pid);
    char *status = join_path(basepath, "stat");

    FILE *file = fopen(status, "r");

    if (file == NULL) {
        perror("Erro ao abrir arquivo de status");
        return 1;
    }

    char line[256] = {0};

    if(fgets(line, sizeof(line), file) == NULL) {
        perror("Erro! /stat devia ter uma linha.\n");
        fclose(file);
        return 1;
    }

    int start = 0;
    unsigned long int total_active = 0;

    // pula os 13 primeiros itens, pois utime fica na posicao 14 do /stat
    for(int c = 0; c < 13; c++) {
        while(line[start++] != ' ');
    }

    // soma #14, #15, #16 e #17 para contabilizar total ativo
    for(int c = 0; c < 4; c++) {
        int end = start;
        while(line[++end] != ' ');

        char slice[end - start + 1];
        memcpy(slice, &line[start], end - start);
        slice[end - start] = '\0';

        unsigned long int time = strtol(slice, NULL, 10);
        total_active += time;
        start = end + 1;
    }

    // pula mais 4 itens, pois starttime fica na posicao 22.
    for(int c = 0; c < 4; c++) {
        while(line[start++] != ' ');
    }

    int end = start;
    while(line[++end] != ' ');

    char slice[end - start + 1];
    memcpy(slice, &line[start], end - start);
    slice[end - start] = '\0';

    p->active_time = total_active;
    p->start_time = strtol(slice, NULL, 10);

    fclose(file);
    return 0;
}
