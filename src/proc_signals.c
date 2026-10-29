#define _POSIX_C_SOURCE 200809L

#include "proc_signals.h"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int send_process_signal(const char *pid, int signal)
{
    if (pid == NULL || pid[0] == '\0')
    {
        errno = EINVAL;
        return -1;
    }

    char *end;
    errno = 0;

    long value = strtol(pid, &end, 10);

    if (errno != 0 || *end != '\0' ||
        value <= 0 || value > INT_MAX)
    {
        errno = EINVAL;
        return -1;
    }

    if (signal != SIGTERM &&
        signal != SIGSTOP &&
        signal != SIGCONT &&
        signal != SIGKILL)
    {
        errno = EINVAL;
        return -1;
    }

    return kill((pid_t)value, signal);
}