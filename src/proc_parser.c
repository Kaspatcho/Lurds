#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proc_parser.h"
#include "utils.h"
#include "app_env.h"
#include <pwd.h>

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
        line[strcspn(line, "\r\n")] = '\0';

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
        perror("Erro ao abrir arquivo comm");
        return 1;
    }
 
    char command[256] = {0};
 
    if (fgets(command, sizeof(command), file) == NULL) {
        perror("Erro ao ler arquivo comm");
        fclose(file);
        return 1;
    }
 
    fclose(file);
 
    command[strcspn(command, "\r\n")] = '\0';
 
    p->name = calloc(strlen(command) + 1, sizeof(char));
 
    if (p->name == NULL) {
        return 1;
    }
 
    strcpy(p->name, command);
 
    return 0;
}

int read_stat(Process *p)
{
    char *basepath = join_path(PROC_PATH, p->pid);
    char *status = join_path(basepath, "stat");

    FILE *file = fopen(status, "r");

    if (file == NULL) {
        perror("Erro ao abrir arquivo stat");
        return 1;
    }

    char line[1024] = {0};

    if(fgets(line, sizeof(line), file) == NULL) {
        perror("Erro! /stat devia ter uma linha.\n");
        fclose(file);
        return 1;
    }

    fclose(file);

    /*
     * O campo 2 (nome do processo) pode conter espaços.
     * Então primeiro encontramos o ')' que fecha o nome.
     */
    char *ptr = strrchr(line, ')');

    if (ptr == NULL) {
        return 1;
    }

    // pula ") "
    ptr += 2;

    unsigned long int utime = 0;
    unsigned long int stime = 0;
    unsigned long int cutime = 0;
    unsigned long int cstime = 0;
    unsigned long int starttime = 0;

    /*
     * Depois do nome:
     *
     * campo 3  = state
     * campo 4  = ppid
     * ...
     * campo 14 = utime
     * campo 15 = stime
     * campo 16 = cutime
     * campo 17 = cstime
     * ...
     * campo 22 = starttime
     */

    for(int field = 3; field <= 22; field++) {

        char slice[64];
        int i = 0;

        while(*ptr != ' ' && *ptr != '\0') {
            slice[i++] = *ptr;
            ptr++;
        }

        slice[i] = '\0';

        if(field == 14)
            utime = strtoul(slice, NULL, 10);

        else if(field == 15)
            stime = strtoul(slice, NULL, 10);

        else if(field == 16)
            cutime = strtoul(slice, NULL, 10);

        else if(field == 17)
            cstime = strtoul(slice, NULL, 10);

        else if(field == 22)
            starttime = strtoul(slice, NULL, 10);

        while(*ptr == ' ')
            ptr++;
    }

    p->active_time = utime + stime + cutime + cstime;
    p->start_time = starttime;

    return 0;
}
 double calculate_cpu(Process *before, Process *after,
                     unsigned long int cpu_before,
                     unsigned long int cpu_after) 
{
    unsigned long int delta_ativo =
        after->active_time - before->active_time; // delta ativo do processo

    unsigned long int delta_total =
        cpu_after - cpu_before; // delta total do sistema

    if (delta_total == 0)
        return 0.0;

    return ((double)delta_ativo / delta_total) * 100.0; // porcentagem de CPU no processo
}
char* get_username(char *uid) // pega o nome do usuario com base no UID (usa getpwuid)
{
    struct passwd *user = getpwuid(strtoul(uid, NULL, 10)); // converte o uid de string para unsigned long int

    if (user == NULL)
        return NULL;

    return user->pw_name;
}
unsigned long int get_cpu_total()
{
    FILE *file = fopen("/proc/stat", "r");

    if (file == NULL) {
        perror("Erro ao abrir /proc/stat");
        return 0;
    }

    char line[256];

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 0;
    }

    fclose(file);

    unsigned long int user;
    unsigned long int nice;
    unsigned long int system;
    unsigned long int idle;

    int result = sscanf(
        line,
        "cpu %lu %lu %lu %lu",
        &user,
        &nice,
        &system,
        &idle
    );

    if (result != 4)
        return 0;

    return user + nice + system + idle;
}

unsigned long int get_memory_total()
{
    FILE *file = fopen("/proc/meminfo", "r");

    if (file == NULL) {
        perror("Erro ao abrir /proc/meminfo");
        return 0;
    }

    char line[256];

    unsigned long int mem_total = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "MemTotal: %lu kB", &mem_total) == 1) {
            break;
        }
    }

    fclose(file);
    return mem_total; // em kB
}

double calculate_memory(Process *p, unsigned long int memory_total)
{
    if (memory_total == 0)
        return 0.0;

    unsigned long int memory_used = strtoul(p->vmrss, NULL, 10);

    return ((double)memory_used / memory_total) * 100.0;
}
