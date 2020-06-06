//
// Created by qijinwen on 4/29/20.
//

#include "CuTest.h"
#include <stdio.h>

CuSuite* StrUtilGetSuite();
CuSuite* itoa_test();

void RunAllTests(void) {
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, StrUtilGetSuite());
    CuSuiteAddSuite(suite, itoa_test());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void) {
    RunAllTests();
}