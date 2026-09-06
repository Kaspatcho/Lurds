#ifndef PROC_PARSER_H
#define PROC_PARSER_H

// read proc files
// - read status
// - read stat
// - read cmdline

// NOTE: ta tudo como char* pq estou com preguica
typedef struct {
    char *name; // program name + args
    char *pid; // process ID
    char *ppid; // parent PID
    char *uid; // user ID
    char *state; // ex. S (sleeping)
    char *vmrss; // ram used

    // Metricas para calculo de CPU:
    //
    // porcentagem CPU = 100 * (delta total - delta idle) / delta total
    // delta total = delta ativo + delta idle
    // pra calcular o delta, medir duas vezes o processo
    // em um intervalo de 1 segundo e calcular a diferenca.
    //
    // para converter de ticks para segundos, dividir por CLK_TCK.
    // ex. int active_time_in_seconds = active_time / CLK_TCK.

    unsigned long int active_time; // tempo ativo em ticks (converter pra segundos)
    unsigned long int start_time; // momento em que o processo nasceu em ticks apos o boot (converter pra segundos)
    // tempo de vida = uptime do sistema (/proc/uptime) - (start_time / CLK_TCK)
    // tempo ocioso = tempo de vida - tempo ativo
} Process;

Process* create_process(char *pid);
int read_status(Process *p);
int read_cmdline(Process *p);
int read_stat(Process *p);

#endif
