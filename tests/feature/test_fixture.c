#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>

#include <cmocka.h>

static void test_fixture_has_expected_content(void **state)
{
    FILE *fixture;
    char content[64] = {0};
    size_t bytes_read;

    (void)state;

    fixture = fopen("tests/fixtures/example.txt", "r");
    assert_non_null(fixture);

    bytes_read = fread(content, 1, sizeof(content) - 1, fixture);
    assert_int_equal(fclose(fixture), 0);
    assert_int_equal(bytes_read, 25);
    assert_string_equal(content, "Fixture example content.\n");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fixture_has_expected_content),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
