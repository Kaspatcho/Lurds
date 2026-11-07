#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>
#include "proc_reader.h"

// teste pra ver se a listagem de processos do /proc funciona
static void test_procs_are_listed(void **state)
{
    (void) state;

    ProcList *dir = get_proc_list();
    assert_non_null(dir);
    assert_int_equal(dir->count, 2);

    int found_41 = 0;
    int found_42 = 0;

    for (size_t i = 0; i < dir->count; i++) {
        Process *d = dir->items[i];
        assert_non_null(d);
        assert_non_null(d->pid);
        if (strcmp(d->pid, "41") == 0) {
            found_41 = 1;
        } else if (strcmp(d->pid, "42") == 0) {
            found_42 = 1;
        }
    }

    assert_true(found_41);
    assert_true(found_42);
    delete_list(&dir);
    assert_null(dir);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_procs_are_listed),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
