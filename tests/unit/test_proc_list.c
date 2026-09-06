#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "proc_reader.h"

// verifica se a ProcList criar a lista corretamente
// com o Process correto
static void test_list_is_created(void **state)
{
    (void)state;
    ProcList *l = create_list();
    assert_int_equal(l->count, 0);
    push_process(l, "41");
    assert_int_equal(l->count, 1);
}

// testa se o redimensionamento do vetor dinamico funciona
// e se os dados sao mantidos
static void test_list_is_redized(void **state)
{
    (void)state;
    ProcList *l = create_list();
    assert_int_equal(l->capacity, 4);
    push_process(l, "41");
    push_process(l, "41");
    push_process(l, "41");
    push_process(l, "41");
    push_process(l, "41");

    assert_int_equal(l->count, 5);
    assert_int_equal(l->capacity, 6);
    assert_string_equal(l->items[0]->pid, "41");
}

// testa se a lista pode ser destruida
static void test_list_is_destroyed(void **state)
{
    (void)state;
    ProcList *l = create_list();
    push_process(l, "41");
    delete_list(&l);
    assert_null(l);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_list_is_created),
        cmocka_unit_test(test_list_is_redized),
        cmocka_unit_test(test_list_is_destroyed),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
