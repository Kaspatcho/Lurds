#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "proc_signals.h"

static void test_empty_pid(void **state)
{
    (void)state;

    errno = 0;
    assert_int_equal(send_process_signal("", SIGTERM), -1);
    assert_int_equal(errno, EINVAL);
}

static void test_invalid_pid(void **state)
{
    (void)state;

    errno = 0;
    assert_int_equal(send_process_signal("abc", SIGTERM), -1);
    assert_int_equal(errno, EINVAL);
}

static void test_zero_pid(void **state)
{
    (void)state;

    errno = 0;
    assert_int_equal(send_process_signal("0", SIGTERM), -1);
    assert_int_equal(errno, EINVAL);
}

static void test_unsupported_signal(void **state)
{
    (void)state;

    errno = 0;
    assert_int_equal(send_process_signal("123", SIGUSR1), -1);
    assert_int_equal(errno, EINVAL);
}

static void test_send_sigterm(void **state)
{
    (void)state;

    pid_t child = fork();
    assert_true(child >= 0);

    if (child == 0)
    {
        for (;;)
            pause();
    }

    char pid[32];
    snprintf(pid, sizeof(pid), "%ld", (long)child);

    assert_int_equal(send_process_signal(pid, SIGTERM), 0);

    int status;
    assert_int_equal(waitpid(child, &status, 0), child);
    assert_true(WIFSIGNALED(status));
    assert_int_equal(WTERMSIG(status), SIGTERM);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_empty_pid),
        cmocka_unit_test(test_invalid_pid),
        cmocka_unit_test(test_zero_pid),
        cmocka_unit_test(test_unsupported_signal),
        cmocka_unit_test(test_send_sigterm),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}