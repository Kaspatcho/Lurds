#ifndef PROC_HEADER_H
#define PROC_HEADER_H

/*
# proc_reader
- list procs (from /proc and from /tests/fixture/procs for testing)
- read proc files (status, stat and cmdline for now)

# proc_parser
- turn proc files into a Process Struct
- make dynamic vector of Process called ProcList
*/

// simple linked list
typedef struct ProcList {
    char *pid; // proc PID/name of the directory
    struct ProcList *next;
} ProcList;

// list procs
ProcList* _make_list(char *pid);
ProcList *_add_proc(ProcList* list, char *pid);
ProcList* _get_proc_list(char *path);
ProcList* get_proc_list();

// read proc files
// - read status
// - read stat
// - read cmdline

char* read_files(char *pid);

#endif
