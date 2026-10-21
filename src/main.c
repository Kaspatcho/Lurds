#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "proc_table.h"
#include "proc_reader.h"
#include "proc_parser.h"

void *update_table_info();
void update_proc_values(ProcList **procs_before, ProcList **procs_after, unsigned long int *cpu_before, unsigned long int *cpu_after);

TableInfo table_info;
char search_query[64] = ""; // armazena string da pesquisa

int main(void)
{
    // Inicializa a estrutura global e o mutex
    table_info.procs_before = NULL;
    table_info.procs_after = NULL;
    table_info.dados_prontos = 0;
    pthread_mutex_init(&table_info.mutex, NULL);

    int selected_index = 0;
    int running = 1;
    unsigned long int memory_total = get_memory_total();

    // Cria a thread secundária para rodar em paralelo
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, update_table_info, NULL) != 0)
    {
        perror("Erro ao criar a thread");
        return 1;
    }

    // calcula a primeira vez de forma sincrona. O resto e atualizado pela thread em paralelo
    ProcList *procs_before = get_proc_list();
    unsigned long int cpu_before = get_cpu_total();
    sleep(1);
    ProcList *procs_after = get_proc_list();
    unsigned long int cpu_after = get_cpu_total();

    while (running)
    {
        update_proc_values(&procs_before, &procs_after, &cpu_before, &cpu_after);

        if (procs_after == NULL)
        {
            printf("Processo ainda nao carregado. Esperando mais um pouco.\n");
            sleep(1);
            continue;
        }

        /* Mantém a seleção dentro dos limites */
        if (procs_after->count == 0)
        {
            selected_index = 0;
        }
        else if (selected_index >= (int)procs_after->count)
        {
            selected_index = (int)procs_after->count - 1;
        }

        printf("\033[2J\033[H");
        fflush(stdout);

        print_proc_table(
            procs_before,
            procs_after,
            cpu_before,
            cpu_after,
            memory_total,
            selected_index,
            search_query);

        printf("\n[w] Subir | [s] Descer | [f] Buscar | [c] Limpar Busca | [q] Sair\n");
        printf("Atualizando automaticamente a cada segundo...\n");
        printf("Processos encontrados: %zu\n", procs_after->count);

        fflush(stdout);

        int tecla = getch_timeout();

        if (tecla == 'q' || tecla == 'Q')
        {
            running = 0;
        }
        else if (tecla == 'f' || tecla == 'F') //abre a busca de processos
        {
            printf("\033[25;1H\033[KDigite o PID ou o nome do processo para buscar: ");
            fflush(stdout);
            
            int idx = 0;
            search_query[0] = '\0'; // limpa a busca antes de começar a digitar

            while (1) {
                int c = getchar();
                if (c == '\n' || c == '\r') {
                    break;
                } else if (c == 127 || c == 8) { // backspace
                    if (idx > 0) {
                        idx--;
                        search_query[idx] = '\0';
                    }
                } else if (c >= 32 && c < 126 && idx <63) {
                    search_query[idx++] = (char)c;
                    search_query[idx] = '\0';
                }
            }
        } else if (tecla == 'c' || tecla == 'C'){
            search_query[0] = '\0'; // limpa a busca
        }
        else if ((tecla == 'w' || tecla == 'W') && selected_index > 0)
        {
            selected_index--;
        }
        else if ((tecla == 's' || tecla == 'S') && selected_index + 1 < (int)procs_after->count)
        {
            selected_index++;
        }
    }

    pthread_cancel(thread_id); // Encerra a thread de segundo plano
    pthread_join(thread_id, NULL);
    pthread_mutex_destroy(&table_info.mutex);

    if (procs_before)
        delete_list(&procs_before);
    if (procs_after)
        delete_list(&procs_after);

    printf("\033[2J\033[H");
    fflush(stdout);
    return 0;
}

void *update_table_info()
{
    while (1)
    {
        // Coleta os dados locais dinamicamente
        ProcList *local_before = get_proc_list();
        unsigned long int local_cpu_before = get_cpu_total();

        sleep(1); // O sleep acontece APENAS nesta thread, sem travar a interface!

        ProcList *local_after = get_proc_list();
        unsigned long int local_cpu_after = get_cpu_total();

        // Passa os ponteiros dinâmicos para a estrutura global com segurança
        pthread_mutex_lock(&table_info.mutex);

        // Libera a memória dinâmica das medições anteriores antes de receber as novas
        if (table_info.procs_before)
            delete_list(&table_info.procs_before);
        if (table_info.procs_after)
            delete_list(&table_info.procs_after);

        table_info.procs_before = local_before;
        table_info.procs_after = local_after;
        table_info.cpu_before = local_cpu_before;
        table_info.cpu_after = local_cpu_after;
        table_info.dados_prontos = 1;

        pthread_mutex_unlock(&table_info.mutex);
    }
}

void update_proc_values(ProcList **procs_before, ProcList **procs_after, unsigned long int *cpu_before, unsigned long int *cpu_after)
{
    // Verifica rapidamente se há novos dados dinâmicos disponíveis
    pthread_mutex_lock(&table_info.mutex);
    if (table_info.dados_prontos)
    {
        delete_list(procs_before);
        delete_list(procs_after);

        *procs_before = table_info.procs_before;
        *procs_after = table_info.procs_after;
        *cpu_before = table_info.cpu_before;
        *cpu_after = table_info.cpu_after;

        // Retira da estrutura global para que a Main seja dona dessas listas agora
        table_info.procs_before = NULL;
        table_info.procs_after = NULL;
        table_info.dados_prontos = 0;
    }

    pthread_mutex_unlock(&table_info.mutex);
}
