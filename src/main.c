#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include "proc_reader.h"
#include "proc_parser.h"

#define BG_AMARELO "\033[43m"
#define TEXTO_PRETO "\033[30m"
#define RESET "\033[0m"

void print_proc_table(ProcList *procs_before, ProcList *procs_after, unsigned long int cpu_before, unsigned long int cpu_after, unsigned long int memory_total, int selected_index);
char getch(void);

int main(void)
{
    int selected_index = 0;

    ProcList *procs_before = get_proc_list();
    unsigned long int cpu_before = get_cpu_total();

    sleep(1);

    ProcList *procs_after = get_proc_list();
    unsigned long int cpu_after = get_cpu_total();
    unsigned long int memory_total = get_memory_total();

    while (1)
    {
        printf("\033[H\033[J");

        print_proc_table(procs_before, procs_after, cpu_before, cpu_after, memory_total, selected_index);

        fflush(stdout);
        printf("\n[w] Subir | [s] Descer | [q] Sair");
        fflush(stdout);

        char tecla = getch();

        // Processa o comando
        if (tecla == 'w' || tecla == 'W')
        {
            if (selected_index > 0)
            {
                selected_index--;
            }
        }
        else if (tecla == 's' || tecla == 'S')
        {
            selected_index++;

            if (selected_index >= (int)procs_after->count)
            {
                selected_index = procs_after->count - 1;
            }
        }
        else if (tecla == 'q' || tecla == 'Q')
        {
            break;
        }
    }

    delete_list(&procs_before);
    delete_list(&procs_after);

    printf("\033[2J\033[H");
    fflush(stdout);
    return 0;
}

void print_proc_table(ProcList *procs_before, ProcList *procs_after, unsigned long int cpu_before, unsigned long int cpu_after, unsigned long int memory_total, int selected_index)
{
    printf("%-28s %7s %7s %-10s %-15s %8s %8s %13s %12s\n",
           "Name", "PID", "PPID", "USER", "State", "CPU", "MEM", "Active Time", "VmRSS");

    for (size_t i = 0; i < procs_after->count; i++)
    {
        Process *p = procs_after->items[i];
        char *username = get_username(p->uid);
        if (username == NULL)
            username = "unknown";

        double cpu = 0.0;
        for (size_t j = 0; j < procs_before->count; j++)
        {
            Process *before = procs_before->items[j];
            if (strcmp(before->pid, p->pid) == 0)
            {
                cpu = calculate_cpu(before, p, cpu_before, cpu_after);
                break;
            }
        }
        double memory = calculate_memory(p, memory_total);

        if (i == (size_t)selected_index)
        {
            printf("%s%s%-28.28s %7.7s %7.7s %-10.10s %-15.15s %7.2f%% %7.2f%% %13lu %12.12s%s\n",
                   BG_AMARELO, TEXTO_PRETO,
                   p->name, p->pid, p->ppid, username, p->state, cpu, memory, p->active_time, p->vmrss,
                   RESET);
        }
        else
        {
            printf("%-28.28s %7.7s %7.7s %-10.10s %-15.15s %7.2f%% %7.2f%% %13lu %12.12s\n",
                   p->name, p->pid, p->ppid, username, p->state, cpu, memory, p->active_time, p->vmrss);
        }
    }
}

char getch(void) {
    struct termios old_opts, new_opts;

    if (tcgetattr(STDIN_FILENO, &old_opts) < 0) {
        return -1;
    }

    new_opts = old_opts;
    new_opts.c_lflag &= ~(ICANON | ECHO);

    new_opts.c_cc[VMIN] = 1;
    new_opts.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);

    char ch = 0;
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_opts);

    return ch;
}
