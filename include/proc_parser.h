#ifndef PROC_PARSER_H
#define PROC_PARSER_H

// read proc files
// - read status
// - read stat
// - read cmdline

// NOTE: ta tudo como char* pq estou com preguica
typedef struct {
    char *name; // program name (TODO)
    char **args; // array of char* (TODO)
    char *pid; // process ID
    char *ppid; // parent PID
    char *uid; // user ID
    char *state;
    char *vmrss; // ram used
    char *cpu_p; // CPU percentage (TODO)
} Process;

Process* create_process(char *pid);
int read_status(Process *p);

#endif
