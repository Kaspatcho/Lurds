#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <cmocka.h>

#include "example.h"

// teste simples de exemplo
static void test_add_numbers_returns_sum(void **state)
{
    (void)state;

    assert_int_equal(add_numbers(2, 3), 5);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add_numbers_returns_sum),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
