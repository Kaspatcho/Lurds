#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "app_env.h"
#include "proc_parser.h"
#include "utils.h"

static void replace_string(char **destination, char *value)
{
    if (value == NULL)
    {
        return;
    }

    free(*destination);
    *destination = value;
}

void delete_process(Process **process)
{
    if (process == NULL || *process == NULL)
    {
        return;
    }

    Process *current = *process;
    free(current->pid);
    free(current->name);
    free(current->ppid);
    free(current->uid);
    free(current->state);
    free(current->vmrss);
    free(current);
    *process = NULL;
}

Process *create_process(const char *pid)
{
    if (!is_numeric(pid))
    {
        return NULL;
    }

    Process *process = calloc(1, sizeof(*process));
    if (process == NULL)
    {
        return NULL;
    }

    process->pid = malloc(strlen(pid) + 1);
    if (process->pid == NULL)
    {
        delete_process(&process);
        return NULL;
    }
    strcpy(process->pid, pid);

    /* A process that exits while /proc is being read must not enter the list. */
    if (read_status(process) != 0 ||
        (read_cmdline(process) != 0 && read_comm(process) != 0) ||
        read_stat(process) != 0)
    {
        delete_process(&process);
        return NULL;
    }

    return process;
}

int read_status(Process *process)
{
    if (process == NULL || process->pid == NULL)
    {
        return 1;
    }

    char *basepath = join_path(PROC_PATH, process->pid);
    char *status_path = join_path(basepath, "status");
    free(basepath);
    if (status_path == NULL)
    {
        return 1;
    }

    FILE *file = fopen(status_path, "r");
    free(status_path);
    if (file == NULL)
    {
        return 1;
    }

    char line[256];
    int has_vmrss = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\r\n")] = '\0';

        if (startswith(line, "State:"))
        {
            replace_string(&process->state, str_slice(line, 7));
        }
        else if (startswith(line, "PPid:"))
        {
            replace_string(&process->ppid, str_slice(line, 6));
        }
        else if (startswith(line, "Uid:"))
        {
            replace_string(&process->uid, str_slice_until(line, 5, '\t'));
        }
        else if (startswith(line, "VmRSS:"))
        {
            char *vmrss = str_slice(line, 11);
            if (vmrss == NULL)
            {
                fclose(file);
                return 1;
            }

            replace_string(&process->vmrss, vmrss);
            has_vmrss = 1;
            break;
        }
    }

    if (!has_vmrss)
    {
        char *vmrss = malloc(sizeof("0 kB"));
        if (vmrss == NULL)
        {
            fclose(file);
            return 1;
        }
        strcpy(vmrss, "0 kB");
        replace_string(&process->vmrss, vmrss);
    }

    fclose(file);
    return 0;
}

int read_cmdline(Process *process)
{
    if (process == NULL || process->pid == NULL)
    {
        return 1;
    }

    char *basepath = join_path(PROC_PATH, process->pid);
    char *cmdline_path = join_path(basepath, "cmdline");
    free(basepath);
    if (cmdline_path == NULL)
    {
        return 1;
    }

    FILE *file = fopen(cmdline_path, "r");
    free(cmdline_path);
    if (file == NULL)
    {
        return 1;
    }

    char command[256];
    size_t bytes_read = fread(command, 1, sizeof(command) - 1, file);
    int read_failed = ferror(file);
    fclose(file);

    if (read_failed || bytes_read == 0)
    {
        return 1;
    }

    command[bytes_read] = '\0';
    for (size_t i = 0; i < bytes_read; i++)
    {
        if (command[i] == '\0')
        {
            command[i] = ' ';
        }
    }

    while (bytes_read > 0 && isspace((unsigned char)command[bytes_read - 1]))
    {
        bytes_read--;
    }
    command[bytes_read] = '\0';

    if (bytes_read == 0)
    {
        return 1;
    }

    char *name = malloc(bytes_read + 1);
    if (name == NULL)
    {
        return 1;
    }
    memcpy(name, command, bytes_read + 1);
    replace_string(&process->name, name);

    return 0;
}

int read_comm(Process *process)
{
    if (process == NULL || process->pid == NULL)
    {
        return 1;
    }

    char *basepath = join_path(PROC_PATH, process->pid);
    char *comm_path = join_path(basepath, "comm");
    free(basepath);
    if (comm_path == NULL)
    {
        return 1;
    }

    FILE *file = fopen(comm_path, "r");
    free(comm_path);
    if (file == NULL)
    {
        return 1;
    }

    char command[256];
    if (fgets(command, sizeof(command), file) == NULL)
    {
        fclose(file);
        return 1;
    }
    fclose(file);

    command[strcspn(command, "\r\n")] = '\0';
    char *name = malloc(strlen(command) + 1);
    if (name == NULL)
    {
        return 1;
    }
    strcpy(name, command);
    replace_string(&process->name, name);

    return 0;
}

int read_stat(Process *process)
{
    if (process == NULL || process->pid == NULL)
    {
        return 1;
    }

    char *basepath = join_path(PROC_PATH, process->pid);
    char *stat_path = join_path(basepath, "stat");
    free(basepath);
    if (stat_path == NULL)
    {
        return 1;
    }

    FILE *file = fopen(stat_path, "r");
    free(stat_path);
    if (file == NULL)
    {
        return 1;
    }

    char line[4096];
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 1;
    }
    fclose(file);

    const char *cursor = strrchr(line, ')');
    if (cursor == NULL)
    {
        return 1;
    }
    cursor++;

    unsigned long int utime = 0;
    unsigned long int stime = 0;
    unsigned long int cutime = 0;
    unsigned long int cstime = 0;
    unsigned long int starttime = 0;

    for (int field = 3; field <= 22; field++)
    {
        while (isspace((unsigned char)*cursor))
        {
            cursor++;
        }
        if (*cursor == '\0')
        {
            return 1;
        }

        int is_time_field = field >= 14 && field <= 17;
        if (is_time_field || field == 22)
        {
            errno = 0;
            char *end = NULL;
            unsigned long int value = strtoul(cursor, &end, 10);
            if (errno == ERANGE || end == cursor ||
                (*end != '\0' && !isspace((unsigned char)*end)))
            {
                return 1;
            }

            if (field == 14)
                utime = value;
            else if (field == 15)
                stime = value;
            else if (field == 16)
                cutime = value;
            else if (field == 17)
                cstime = value;
            else
                starttime = value;

            cursor = end;
        }
        else
        {
            while (*cursor != '\0' && !isspace((unsigned char)*cursor))
            {
                cursor++;
            }
        }
    }

    process->active_time = utime + stime + cutime + cstime;
    process->start_time = starttime;
    return 0;
}

double calculate_cpu(Process *before, Process *after,
                     unsigned long int cpu_before,
                     unsigned long int cpu_after)
{
    if (before == NULL || after == NULL ||
        after->active_time < before->active_time || cpu_after <= cpu_before)
    {
        return 0.0;
    }

    unsigned long int delta_active = after->active_time - before->active_time;
    unsigned long int delta_total = cpu_after - cpu_before;
    return ((double)delta_active / delta_total) * 100.0;
}

char *get_username(const char *uid)
{
    if (!is_numeric(uid))
    {
        return NULL;
    }

    errno = 0;
    char *end = NULL;
    unsigned long value = strtoul(uid, &end, 10);
    if (errno == ERANGE || *end != '\0' || value > (unsigned long)(uid_t)-1)
    {
        return NULL;
    }

    struct passwd *user = getpwuid((uid_t)value);
    return user == NULL ? NULL : user->pw_name;
}

unsigned long int get_cpu_total(void)
{
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
    {
        return 0;
    }

    char line[256];
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 0;
    }
    fclose(file);

    unsigned long int user = 0;
    unsigned long int nice = 0;
    unsigned long int system = 0;
    unsigned long int idle = 0;
    unsigned long int iowait = 0;
    unsigned long int irq = 0;
    unsigned long int softirq = 0;
    unsigned long int steal = 0;

    int fields = sscanf(line, "cpu %lu %lu %lu %lu %lu %lu %lu %lu",
                        &user, &nice, &system, &idle, &iowait, &irq,
                        &softirq, &steal);
    if (fields < 4)
    {
        return 0;
    }

    return user + nice + system + idle + iowait + irq + softirq + steal;
}

unsigned long int get_memory_total(void)
{
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL)
    {
        return 0;
    }

    char line[256];
    unsigned long int memory_total = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "MemTotal: %lu kB", &memory_total) == 1)
        {
            break;
        }
    }

    fclose(file);
    return memory_total;
}

double calculate_memory(Process *process, unsigned long int memory_total)
{
    if (process == NULL || process->vmrss == NULL || memory_total == 0)
    {
        return 0.0;
    }

    return ((double)strtoul(process->vmrss, NULL, 10) / memory_total) * 100.0;
}
