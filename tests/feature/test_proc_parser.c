#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>
#include "proc_parser.h"

static void test_process_is_generated(void **state)
{
    (void) state;
    Process *p = create_process("41");

    assert_string_equal(p->name, "sleep 300");
    assert_string_equal(p->state, "S (sleeping)");
    assert_string_equal(p->ppid, "1");
    assert_string_equal(p->uid, "0");
    assert_string_equal(p->vmrss, "1216 kB");
    assert_int_equal(p->active_time, 220);
    assert_int_equal(p->start_time, 100821);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_process_is_generated),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
