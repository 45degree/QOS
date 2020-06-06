#include "CuTest.h"
#include "show.h"

#include <stdio.h>

void itoa_test1(CuTest* tc) {
    char temp[256];
    itoa(temp, 12);
    CuAssertStrEquals(tc, "0xC", temp);

    itoa(temp, 16);
    CuAssertStrEquals(tc, "0x10", temp);
}

CuSuite* itoa_test() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, itoa_test1);
    return suite;
}