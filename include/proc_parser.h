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
    char *cpu_p; // CPU percentage (TODO)
} Process;

Process* create_process(char *pid);
int read_status(Process *p);
int read_cmdline(Process *p);

#endif
