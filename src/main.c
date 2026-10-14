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
 
void print_proc_table(
    ProcList *procs_before,
    ProcList *procs_after,
    unsigned long int cpu_before,
    unsigned long int cpu_after,
    unsigned long int memory_total,
    int selected_index
);
 
int getch_timeout(void);
 
int main(void)
{
    int selected_index = 0;
    int running = 1;
 
    ProcList *procs_before = get_proc_list();
 
    if (procs_before == NULL)
    {
        fprintf(stderr, "Erro ao carregar os processos.\n");
        return 1;
    }
 
    unsigned long int cpu_before = get_cpu_total();
 
    /* Aguarda a primeira medição de CPU */
    sleep(1);
 
    while (running)
    {
        ProcList *procs_after = get_proc_list();
 
        if (procs_after == NULL)
        {
            fprintf(stderr, "Erro ao atualizar os processos.\n");
            break;
        }
 
        unsigned long int cpu_after = get_cpu_total();
        unsigned long int memory_total = get_memory_total();
 
        /* Mantém a seleção dentro dos limites */
        if (procs_after->count == 0)
        {
            selected_index = 0;
        }
        else if (selected_index >= (int)procs_after->count)
        {
            selected_index = (int)procs_after->count - 1;
        }
 
        printf("\033[H\033[J");
 
        print_proc_table(
            procs_before,
            procs_after,
            cpu_before,
            cpu_after,
            memory_total,
            selected_index
        );
 
        printf("\n[w] Subir | [s] Descer | [q] Sair\n");
        printf("Atualizando automaticamente a cada segundo...\n");
        printf("Processos encontrados: %zu\n", procs_after->count);
 
        fflush(stdout);
 
        int tecla = getch_timeout();
 
        if (tecla == 'q' || tecla == 'Q')
        {
            running = 0;
        }
        else if ((tecla == 'w' || tecla == 'W')
&& selected_index > 0)
        {
            selected_index--;
        }
        else if ((tecla == 's' || tecla == 'S')
&& selected_index + 1 < (int)procs_after->count)
        {
            selected_index++;
        }
 
        /* Atualiza a referência para o próximo cálculo */
        delete_list(&procs_before);
 
        procs_before = procs_after;
        cpu_before = cpu_after;
    }
 
    delete_list(&procs_before);
 
    printf("%s\033[1EOficialmente saindo...\n", RESET);
 
    return 0;
}
 
void print_proc_table(
    ProcList *procs_before,
    ProcList *procs_after,
    unsigned long int cpu_before,
    unsigned long int cpu_after,
    unsigned long int memory_total,
    int selected_index
)
{
    printf(
        "%-28s %7s %7s %-10s %-15s %8s %8s %13s %12s\n",
        "Name", "PID", "PPID", "USER", "State",
        "CPU", "MEM", "Active Time", "VmRSS"
    );
 
    for (size_t i = 0; i < procs_after->count; i++)
    {
        Process *p = procs_after->items[i];
 
        char *username = get_username(p->uid);
 
        if (username == NULL)
        {
            username = "unknown";
        }
 
        double cpu = 0.0;
 
        for (size_t j = 0; j < procs_before->count; j++)
        {
            Process *before = procs_before->items[j];
 
            if (strcmp(before->pid, p->pid) == 0)
            {
                cpu = calculate_cpu(
                    before,
                    p,
                    cpu_before,
                    cpu_after
                );
 
                break;
            }
        }
 
        double memory = 0.0;
 
        if (memory_total > 0)
        {
            memory = calculate_memory(p, memory_total);
        }
 
        const char *name = p->name ? p->name : "unknown";
        const char *ppid = p->ppid ? p->ppid : "-";
        const char *state = p->state ? p->state : "-";
        const char *vmrss = p->vmrss ? p->vmrss : "0 kB";
 
        if (i == (size_t)selected_index)
        {
            printf(
                "%s%s%-28.28s %7.7s %7.7s %-10.10s "
                "%-15.15s %7.2f%% %7.2f%% %13lu %12.12s%s\n",
                BG_AMARELO,
                TEXTO_PRETO,
                name,
                p->pid,
                ppid,
                username,
                state,
                cpu,
                memory,
                p->active_time,
                vmrss,
                RESET
            );
        }
        else
        {
            printf(
                "%-28.28s %7.7s %7.7s %-10.10s "
                "%-15.15s %7.2f%% %7.2f%% %13lu %12.12s\n",
                name,
                p->pid,
                ppid,
                username,
                state,
                cpu,
                memory,
                p->active_time,
                vmrss
            );
        }
    }
}
 
int getch_timeout(void)
{
    struct termios old_opts;
    struct termios new_opts;
 
    if (tcgetattr(STDIN_FILENO, &old_opts) < 0)
    {
        return 0;
    }
 
    new_opts = old_opts;
 
    /* Permite ler teclas sem Enter e sem exibi-las na tela */
    new_opts.c_lflag &= ~(ICANON | ECHO);
 
    /* Espera no máximo 1 segundo por uma tecla */
    new_opts.c_cc[VMIN] = 0;
    new_opts.c_cc[VTIME] = 10;
 
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_opts) < 0)
    {
        return 0;
    }
 
    char ch = 0;
    ssize_t bytes_read = read(STDIN_FILENO, &ch, 1);
 
    /* Restaura as configurações originais do terminal */
    tcsetattr(STDIN_FILENO, TCSANOW, &old_opts);
 
    if (bytes_read == 1)
    {
        return (unsigned char)ch;
    }
 
    return 0;
}