// contem funcoes base para criar struct Process
// a partir de um arquivo /proc/{PID}
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

// cria uma nova struct Process com base no PID passado.
// funcao que deve ser usada principalmente, pois chama as demais desse modulo
Process* create_process(char *pid);

// le o arquivo /proc/{PID}/status e atualiza o Process.
// tem informacoes como PID, PPID, UID, state e VmRSS
// (ver o ./tests/fixtures/proc/41/status do fixture pra mais detalhes)
int read_status(Process *p);

// le o arquivo /proc/{PID}/cmdline e atualiza o Process.
// tem o nome e argumentos do comando que iniciou o processo.
// pode vir vazio caso o processo seja uma kernel thread ou um zombie process.
int read_cmdline(Process *p);

// le o arquivo /proc/{PID}/comm e atualiza o Process.
// tem o nome do arquivo, usado em caso de erro em /cmdline.
int read_comm(Process *p);

// le o arquivo /proc/{PID}/stat e atualiza o Process.
// tem informacoes da CPU. Usado para calcular active_time e start_time.
int read_stat(Process *p);

#endif
