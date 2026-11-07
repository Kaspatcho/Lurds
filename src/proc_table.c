#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <limits.h>

#include "proc_table.h"
#include "proc_reader.h"
#include "proc_parser.h"
#include "utils.h"

#define BG_AMARELO "\033[43m"
#define TEXTO_PRETO "\033[30m"
#define RESET "\033[0m"
#define TEXTO_CIANO "\033[1;36m"
#define TEXTO_VERDE "\033[32m"
#define TEXTO_AZUL "\033[1;34m"

#define MAX_LINES 15

typedef struct {
    int index;
    double cpu;
} ProcessSortItem;

void merge(ProcessSortItem *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    ProcessSortItem *L = malloc(sizeof(ProcessSortItem) * n1);
    ProcessSortItem *R = malloc(sizeof(ProcessSortItem) * n2);
    if (!L || !R) { free(L); free(R); return; }

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].cpu >= R[j].cpu) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort(ProcessSortItem *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}


int process_terminated(
    ProcList *procs_before,
    ProcList *procs_after,
    const char *pid
)
{
    if (procs_before == NULL || pid == NULL)
    {
        return 0;
    }

    /* Verifica se o processo existia na lista anterior. */
    int existed_before = 0;

    for (size_t i = 0; i < procs_before->count; i++)
    {
        Process *p = procs_before->items[i];

        if (p != NULL && p->pid != NULL &&
            strcmp(p->pid, pid) == 0)
        {
            existed_before = 1;
            break;
        }
    }

    if (!existed_before)
    {
        return 0;
    }

    /* Se a lista atual ainda não existe, não confirma o término. */
    if (procs_after == NULL)
    {
        return 0;
    }

    /* Se o PID não aparece na lista atual, ele desapareceu. */
    for (size_t i = 0; i < procs_after->count; i++)
    {
        Process *p = procs_after->items[i];

        if (p != NULL && p->pid != NULL &&
            strcmp(p->pid, pid) == 0)
        {
            return 0;
        }
    }

    return 1;
}

int process_matches_filter(const Process *process, const char *search_query)
{
    if (process == NULL)
    {
        return 0;
    }

    const char *query = search_query == NULL ? "" : search_query;
    if (query[0] == '\0')
    {
        return 1;
    }

    if (strcmp(query, ":run") == 0)
    {
        return process->state != NULL &&
               (process->state[0] == 'R' || process->state[0] == 'r');
    }

    if (strcmp(query, ":me") == 0)
    {
        if (process->uid == NULL)
        {
            return 0;
        }

        char current_uid[32];
        snprintf(current_uid, sizeof(current_uid), "%lu",
                 (unsigned long)getuid());
        return strcmp(process->uid, current_uid) == 0;
    }

    if (query[0] == ':')
    {
        const char *target_pid = query + 1;
        return process->pid != NULL && is_numeric(target_pid) &&
               strcmp(process->pid, target_pid) == 0;
    }

    if ((process->name != NULL && strstr(process->name, query) != NULL) ||
        (process->pid != NULL && strstr(process->pid, query) != NULL))
    {
        return 1;
    }

    char *username = get_username(process->uid);
    return username != NULL && strstr(username, query) != NULL;
}

size_t get_filtered_process_count(const ProcList *processes, const char *search_query)
{
    if (processes == NULL)
    {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < processes->count; i++)
    {
        const Process *process = processes->items[i];
        if (process != NULL && process->pid != NULL &&
            process_matches_filter(process, search_query))
        {
            count++;
        }
    }

    return count;
}

int clamp_selected_index(int selected_index, size_t item_count)
{
    if (selected_index < 0 || item_count == 0)
    {
        return 0;
    }

    if ((size_t)selected_index < item_count)
    {
        return selected_index;
    }

    return item_count > (size_t)INT_MAX ? INT_MAX : (int)item_count - 1;
}

int get_window_start(int selected_index, size_t item_count, int max_lines)
{
    if (item_count == 0 || max_lines <= 0)
    {
        return 0;
    }

    selected_index = clamp_selected_index(selected_index, item_count);
    int lines = item_count < (size_t)max_lines ? (int)item_count : max_lines;
    int start = selected_index - lines / 2;

    if (start < 0)
    {
        return 0;
    }

    int last_start = item_count > (size_t)INT_MAX ? INT_MAX - lines : (int)item_count - lines;
    return start > last_start ? last_start : start;
}


void print_proc_table(
    ProcList *procs_before,
    ProcList *procs_after,
    unsigned long int cpu_before,
    unsigned long int cpu_after,
    unsigned long int memory_total,
    int selected_index,
    char *search_query)
{
    if (procs_after == NULL || procs_after->count == 0)
    {
        printf("%s", TEXTO_CIANO);
        printf(
            "%-28s %7s %7s %-10s %-15s %8s %8s %13s %12s\n",
            "Name", "PID", "PPID", "USER", "State",
            "CPU", "MEM", "Active Time", "VmRSS");
            printf("%s", RESET);
        printf("Nenhum processo encontrado.\n");
        return;
       
    }

    int current_uid = getuid();

    ProcessSortItem *valid_items = malloc(sizeof(ProcessSortItem) * procs_after->count);
    if (valid_items == NULL)
    {
        return;
    }
    int valid_count = 0;

    for (size_t i = 0; i < procs_after->count; i++)
    {
        Process *p = procs_after->items[i];
        int match = 0;

        char *username = get_username(p->uid);
        if (username == NULL) {
            username = "unknown";
        }

        if (search_query == NULL || strlen(search_query) == 0) 
        { 
            match = 1; // busca vazia considera todos os processos
        }
        else if (strcmp(search_query, ":run") == 0) // filtra apenas os processos em execução
        {
            if (p->state && (p->state[0] == 'R' || p->state[0] == 'r')) {
                match = 1;
            }
        }
        else if (strcmp(search_query, ":me") == 0) // filtra apenas os processos do usuário atual
        {
            char *current_user = getenv("USER");
            if (current_user && current_user[0] != '\0' && strcmp(username, current_user) == 0) {
                match = 1;
            }
        }
        else if (search_query[0] == ':') // Filtro por PID no formato :número (ex: :25)
        {
            const char *target_pid = search_query + 1; 
            
            // Verifica se o que vem depois dos dois pontos é puramente numérico
            int is_valid_num = 1;
            for (int k = 0; target_pid[k] != '\0'; k++) {
                if (target_pid[k] < '0' || target_pid[k] > '9') {
                    is_valid_num = 0;
                    break;
                }
            }

            if (is_valid_num && target_pid[0] != '\0') {
                if (p->pid && strcmp(p->pid, target_pid) == 0) {
                    match = 1;
                }
            }
        }
        else 
        {
            // Busca normal por nome ou usuário (se não usar os comandos com ':')
            if ((p->name && strstr(p->name, search_query) != NULL) ||
                (strstr(username, search_query) != NULL) ||
                (p->pid && strstr(p->pid, search_query) != NULL))
            {
                match = 1;
            }
        }

        if (match) 
        {
            // Calcula a CPU para este processo
            double cpu = 0.0;
            if (procs_before != NULL)
            {
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
            }

            valid_items[valid_count].index = (int)i;
            valid_items[valid_count].cpu = cpu;
            valid_count++;
        }
    }

    // Ordena com base na CPU em ordem decrescente
    if (valid_count > 1) {
        merge_sort(valid_items, 0, valid_count - 1);
    }

    printf("%s", TEXTO_CIANO);
    printf(
        "%-28s %7s %7s %-10s %-15s %8s %8s %13s %12s\n",
        "Name", "PID", "PPID", "USER", "State",
        "CPU", "MEM", "Active Time", "VmRSS");
    printf("%s", RESET);

    if (valid_count == 0)
    {
        printf("Nenhum processo corresponde à busca \"%s\".\n", search_query);
        free(valid_items);
        return;
    }

    int lines = valid_count < MAX_LINES ? valid_count : MAX_LINES;
    int start = selected_index - lines / 2;

    if (start < 0)
        start = 0;

    if (start > valid_count - lines)
        start = valid_count - lines;

    int end = start + lines;

    for (int idx = start; idx < end; idx++)
    {
        int real_i = valid_items[idx].index;
        Process *p = procs_after->items[real_i];
        double cpu = valid_items[idx].cpu;

        char *username = get_username(p->uid);

        if (username == NULL)
        {
            username = "unknown";
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

        int is_current_user = (atoi(p->uid) == current_uid);

        if (idx == selected_index)
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
                RESET);
        }

        else if (is_current_user)
{
    printf("%s", TEXTO_AZUL);

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

    printf("%s", RESET);
}

        else
        {
            printf("%s", TEXTO_VERDE);

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
                vmrss);

            printf("%s", RESET);
        }
    }

    free(valid_items);
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