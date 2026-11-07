#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "utils.h"

static void test_join_path_allocates_for_the_full_path(void **state)
{
    (void)state;
    char base[128];
    memset(base, 'a', sizeof(base) - 1);
    base[sizeof(base) - 1] = '\0';

    char *path = join_path(base, "status");
    assert_non_null(path);
    assert_string_equal(path + strlen(base), "/status");
    free(path);
}

static void test_slice_rejects_invalid_input_without_underflow(void **state)
{
    (void)state;

    assert_null(str_slice(NULL, 0));
    assert_null(str_slice("pid", 4));
    assert_null(str_slice_until(NULL, 0, ':'));
    assert_null(str_slice_until("pid", 4, ':'));
}

static void test_numeric_requires_at_least_one_digit(void **state)
{
    (void)state;

    assert_false(is_numeric(NULL));
    assert_false(is_numeric(""));
    assert_false(is_numeric("42x"));
    assert_true(is_numeric("42"));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_join_path_allocates_for_the_full_path),
        cmocka_unit_test(test_slice_rejects_invalid_input_without_underflow),
        cmocka_unit_test(test_numeric_requires_at_least_one_digit),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
