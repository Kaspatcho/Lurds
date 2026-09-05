#ifndef PROC_READER_H
#define PROC_READER_H

/*
# proc_reader
- list procs (from /proc and from /tests/fixture/procs for testing)

# proc_parser
- read proc files (status, stat and cmdline for now)
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

#endif
