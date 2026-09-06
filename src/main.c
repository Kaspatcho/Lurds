#include <stdio.h>

#include "proc_reader.h"

int main(void)
{
    ProcList* procs = get_proc_list();

    printf("\nName\t\t\t\tPID\tPPID\tUID\tState\t\tActive Time\t\tVmRSS\n");
    for(size_t i = 0; i < procs->count; i++) {
        Process *p = procs->items[i];
        printf("%-30.30s\t%-4.4s\t%-4.4s\t%-4.4s\t%-12.12s\t%-12ld\t\t%-12.12s\n", p->name, p->pid, p->ppid, p->uid, p->state, p->active_time, p->vmrss);
    }

    return 0;
}
