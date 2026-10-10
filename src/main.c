#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "proc_reader.h"
#include "proc_parser.h"


int main(void)
{

    ProcList* procs_before = get_proc_list();
    unsigned long int cpu_before = get_cpu_total();

    sleep(1);

    ProcList* procs_after = get_proc_list();
    unsigned long int cpu_after = get_cpu_total();
    unsigned long int memory_total = get_memory_total();

    printf("%-28s %7s %7s %-10s %-15s %8s %8s %13s %12s\n","Name","PID","PPID","USER","State","CPU","MEM","Active Time","VmRSS");
    for(size_t i = 0; i < procs_after->count; i++) {
        Process *p = procs_after->items[i];
        char *username = get_username(p->uid);
        if(username == NULL)
            username = "unknown";

        double cpu = 0.0;

        for(size_t j = 0; j < procs_before->count; j++) {
            Process *before = procs_before->items[j];
            if(strcmp(before->pid, p->pid) == 0) {
                cpu = calculate_cpu(
                    before,
                    p,
                    cpu_before,
                    cpu_after
                );
                break;
            }
        }
        double memory = calculate_memory(p, memory_total);
        printf("%-28.28s %7.7s %7.7s %-10.10s %-15.15s %7.2f%% %7.2f%% %13lu %12.12s\n",p->name,p->pid,p->ppid,username,p->state,cpu,memory,p->active_time,p->vmrss);
    }
return 0;

}
