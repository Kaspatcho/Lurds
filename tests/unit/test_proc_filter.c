#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>

#include "proc_table.h"

static void test_empty_search_matches_all(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    assert_true(process_matches_filter(&p, ""));
}

static void test_search_matches_pid(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    assert_true(process_matches_filter(&p, "41"));
}

static void test_search_matches_name(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    assert_true(process_matches_filter(&p, "sleep"));
}

static void test_search_without_matches_returns_none(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    assert_false(process_matches_filter(&p, "processo_inexistente"));
}

static void test_run_filter_matches_running_process(void **state)
{
    (void) state;

    Process p = {
        .name = "example",
        .pid = "42",
        .uid = "0",
        .state = "R (running)"
    };

    assert_true(process_matches_filter(&p, ":run"));
}

static void test_run_filter_rejects_sleeping_process(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    assert_false(process_matches_filter(&p, ":run"));
}

static void test_null_process_is_rejected(void **state)
{
    (void) state;

    assert_false(process_matches_filter(NULL, ""));
}

static void test_search_matches_username(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    assert_true(process_matches_filter(&p, "root"));
}

static void test_me_filter_matches_current_user(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    char *original_user = getenv("USER");
    char *saved_user = original_user ? strdup(original_user) : NULL;

    setenv("USER", "root", 1);

    int result = process_matches_filter(&p, ":me");

    if (saved_user != NULL) {
        setenv("USER", saved_user, 1);
        free(saved_user);
    } else {
        unsetenv("USER");
    }

    assert_true(result);
}

static void test_me_filter_rejects_other_user(void **state)
{
    (void) state;

    Process p = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)"
    };

    assert_false(process_matches_filter(&p, ":me"));
}


int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_empty_search_matches_all),
        cmocka_unit_test(test_search_matches_pid),
        cmocka_unit_test(test_search_matches_name),
        cmocka_unit_test(test_search_without_matches_returns_none),
        cmocka_unit_test(test_run_filter_matches_running_process),
        cmocka_unit_test(test_run_filter_rejects_sleeping_process),
        cmocka_unit_test(test_null_process_is_rejected),

        cmocka_unit_test(test_search_matches_username),
        cmocka_unit_test(test_me_filter_matches_current_user),
        cmocka_unit_test(test_me_filter_rejects_other_user),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}