#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>
#include "proc_reader.h"

static void test_procs_are_listed(void **state)
{
    (void) state;

    ProcList* dir = get_proc_list();
    get_proc_list();
    assert_string_equal(dir->items[0]->pid, "41");

    for(size_t i = 0; i < dir->count; i++) {
        Process *d = dir->items[i];
        assert_non_null(d->pid);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_procs_are_listed),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
