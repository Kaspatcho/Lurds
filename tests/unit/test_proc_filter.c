#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include <cmocka.h>

#include "proc_table.h"

static Process process_fixture(void)
{
    Process process = {
        .name = "sleep 300",
        .pid = "41",
        .uid = "0",
        .state = "S (sleeping)",
    };
    return process;
}

static void test_empty_or_null_search_matches_all(void **state)
{
    (void)state;
    Process process = process_fixture();

    assert_true(process_matches_filter(&process, ""));
    assert_true(process_matches_filter(&process, NULL));
}

static void test_search_matches_pid_and_name(void **state)
{
    (void)state;
    Process process = process_fixture();

    assert_true(process_matches_filter(&process, "41"));
    assert_true(process_matches_filter(&process, "sleep"));
    assert_false(process_matches_filter(&process, "processo_inexistente"));
}

static void test_pid_filter_requires_an_exact_numeric_pid(void **state)
{
    (void)state;
    Process process = process_fixture();

    assert_true(process_matches_filter(&process, ":41"));
    assert_false(process_matches_filter(&process, ":4"));
    assert_false(process_matches_filter(&process, ":41x"));
    assert_false(process_matches_filter(&process, ":"));
}

static void test_run_filter_uses_process_state(void **state)
{
    (void)state;
    Process process = process_fixture();

    process.state = "R (running)";
    assert_true(process_matches_filter(&process, ":run"));

    process.state = "S (sleeping)";
    assert_false(process_matches_filter(&process, ":run"));
}

static void test_me_filter_uses_current_uid(void **state)
{
    (void)state;
    Process process = process_fixture();
    char current_uid[32];
    snprintf(current_uid, sizeof(current_uid), "%lu", (unsigned long)getuid());
    process.uid = current_uid;

    assert_true(process_matches_filter(&process, ":me"));
}

static void test_me_filter_rejects_a_different_uid(void **state)
{
    (void)state;
    Process process = process_fixture();
    uid_t other_uid = getuid() == 0 ? 1 : 0;
    char other_uid_text[32];
    snprintf(other_uid_text, sizeof(other_uid_text), "%lu", (unsigned long)other_uid);
    process.uid = other_uid_text;

    assert_false(process_matches_filter(&process, ":me"));
}

static void test_search_matches_the_resolved_username(void **state)
{
    (void)state;
    Process process = process_fixture();
    char current_uid[32];
    snprintf(current_uid, sizeof(current_uid), "%lu", (unsigned long)getuid());
    char *username = get_username(current_uid);
    assert_non_null(username);

    process.uid = current_uid;
    assert_true(process_matches_filter(&process, username));
}

static void test_null_process_is_rejected(void **state)
{
    (void)state;

    assert_false(process_matches_filter(NULL, ""));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_empty_or_null_search_matches_all),
        cmocka_unit_test(test_search_matches_pid_and_name),
        cmocka_unit_test(test_pid_filter_requires_an_exact_numeric_pid),
        cmocka_unit_test(test_run_filter_uses_process_state),
        cmocka_unit_test(test_me_filter_uses_current_uid),
        cmocka_unit_test(test_me_filter_rejects_a_different_uid),
        cmocka_unit_test(test_search_matches_the_resolved_username),
        cmocka_unit_test(test_null_process_is_rejected),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
