#ifndef PROC_READER_H
#define PROC_READER_H
#include "proc_parser.h"

/*
# proc_reader
- list procs (from /proc and from /tests/fixture/procs for testing)

# proc_parser
- read proc files (status, stat and cmdline for now)
- turn proc files into a Process Struct
- make dynamic vector of Process called ProcList
*/

// simple dynamic vector
typedef struct ProcList {
    size_t count;
    size_t capacity;
    Process** items; // array of process
} ProcList;

// list procs
ProcList* create_list();
void push_process(ProcList *list, char *pid);
void resize(ProcList *list);
void delete_list(ProcList **list);
ProcList* get_proc_list();

#endif
