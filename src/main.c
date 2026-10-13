#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "proc_reader.h"
#include "proc_parser.h"

#define BG_AMARELO "\033[43m"
#define TEXTO_PRETO "\033[30m"
#define RESET "\033[0m"

int main(void)
{

inicio:; // Comando vazio para o rótulo ser aceito
    char comando[20];

    ProcList *procs_before = get_proc_list();
    unsigned long int cpu_before = get_cpu_total();

    sleep(1);

    ProcList *procs_after = get_proc_list();
    unsigned long int cpu_after = get_cpu_total();
    unsigned long int memory_total = get_memory_total();

    int selected_index = 0;

    printf("%-28s %7s %7s %-10s %-15s %8s %8s %13s %12s\n", "Name", "PID", "PPID", "USER", "State", "CPU", "MEM", "Active Time", "VmRSS");
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
                cpu = calculate_cpu(
                    before,
                    p,
                    cpu_before,
                    cpu_after);
                break;
            }
        }
        double memory = calculate_memory(p, memory_total);
        printf("%-28.28s %7.7s %7.7s %-10.10s %-15.15s %7.2f%% %7.2f%% %13lu %12.12s\n", p->name, p->pid, p->ppid, username, p->state, cpu, memory, p->active_time, p->vmrss);

        // destacar linha//
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

        while (1)
        {
            printf("\033[H\033[J");

            ProcList *procs_before = get_proc_list();
            unsigned long int cpu_before = get_cpu_total();

            ProcList *procs_after = get_proc_list();
            unsigned long int cpu_after = get_cpu_total();
            unsigned long int memory_total = get_memory_total();

            if (procs_after && procs_after->count > 0)
            {
                if (selected_index >= (int)procs_after->count)
                {
                    selected_index = procs_after->count - 1;
                }
            }
            else
            {
                selected_index = 0;
            }

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

            fflush(stdout);

            delete_list(&procs_before);
            delete_list(&procs_after);

            printf("\n[w] Subir | [s] Descer | [q] Sair -> Comando: ");
            fflush(stdout);

            int tecla = getchar();

            if (tecla != '\n' && tecla != EOF)
            {
                int c;
                while ((c = getchar()) != '\n' && c != EOF)
                    ;
            }

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
            }
            else if (tecla == 'q' || tecla == 'Q')
            {
                goto fim;
            }
        }
    }
fim:
    printf("%s\033[1Eoficialmente saindo...\n", RESET);

    delete_list(&procs_before);
    delete_list(&procs_after);

    // para limpar a tela//
    while (1)
    {
        fflush(stdout);
        if (fgets(comando, sizeof(comando), stdin) == NULL)
        {
            printf("Fim do arquivo detectado (EOF). Saindo...\n");
            break;
        }
        comando[strcspn(comando, "\n")] = '\0';
        if (strcmp(comando, "sair") == 0)
        {
            printf("Saindo do programa...\n");
            continue;
        }
        else if (strcmp(comando, "limpar") == 0)
        {
            printf("\033[2J\033[H");
            fflush(stdout);
            break;
        }
        else if (strlen(comando) > 0)
        {
            printf("Comando desconhecido: '%s'.\n", comando);
            goto inicio;
        }
    }

    return 0;
}
