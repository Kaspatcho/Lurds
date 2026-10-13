#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>
#include "proc_parser.h"

// verifica se a struct Process e gerada corretamente
// a partir do PID 41 (PID da proc falsa em fixtures)
static void test_process_is_generated(void **state)
{
    (void)state;
    Process *p = create_process("41");

    assert_string_equal(p->name, "sleep 300");
    assert_string_equal(p->state, "S (sleeping)");
    assert_string_equal(p->ppid, "1");
    assert_string_equal(p->uid, "0");
    assert_string_equal(p->vmrss, "1216 kB");
    assert_int_equal(p->active_time, 220);
    assert_int_equal(p->start_time, 100821);
}

// verifica se a struct Process e gerada corretamente
// a partir do PID 41 (PID da proc falsa em fixtures)
static void test_zombie_process_is_read(void **state)
{
    (void)state;
    Process *p = create_process("42");

    assert_string_equal(p->name, "sleep");
    assert_string_equal(p->state, "Z (zombie)");
    assert_string_equal(p->ppid, "1");
    assert_string_equal(p->uid, "0");
    assert_string_equal(p->vmrss, "0 kB");
    assert_int_equal(p->active_time, 220);
    assert_int_equal(p->start_time, 100821);
}
// verifica se o UID e convertido corretamente para o nome do usuario
static void test_get_username(void **state)
{
    (void)state;

    char *username = get_username("0");

    assert_non_null(username);
    assert_string_equal(username, "root");
}

// verifica se o total de CPU pode ser lido
static void test_get_cpu_total(void **state)
{
    (void)state;

    unsigned long int cpu_total = get_cpu_total();

    assert_true(cpu_total > 0);
}

// verifica se o calculo de CPU esta correto
static void test_calculate_cpu(void **state)
{
    (void)state;

    Process before;
    Process after;

    before.active_time = 100;
    after.active_time = 150;

    unsigned long int cpu_before = 1000;
    unsigned long int cpu_after = 1100;

    double cpu = calculate_cpu(
        &before,
        &after,
        cpu_before,
        cpu_after);

    // delta ativo = 50
    // delta total = 100
    // CPU = 50 / 100 * 100 = 50%
    assert_float_equal(cpu, 50.0, 0.001);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_process_is_generated),
        cmocka_unit_test(test_zombie_process_is_read),
        cmocka_unit_test(test_get_username),
        cmocka_unit_test(test_get_cpu_total),
        cmocka_unit_test(test_calculate_cpu),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
