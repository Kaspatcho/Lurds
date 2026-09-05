#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>
#include "proc_reader.h"

static void test_fixture_has_expected_content(void **state)
{
    (void) state;

    ProcList* dir = get_proc_list(1);
    for(ProcList *d = dir; d->next != NULL; d = d->next) {
        // pula diretorios "fake" do linux
        // (.. aponta pro diretorio pai e . aponta pro atual)
        if(!strcmp(d->pid, "..") || !strcmp(d->pid, ".")) {
            continue;
        }

        assert_string_equal(d->pid, "41");
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fixture_has_expected_content),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
