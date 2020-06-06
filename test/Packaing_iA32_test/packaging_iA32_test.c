//
// Created by qijinwen on 5/1/20.
//

#include "../CuTest.h"
#include "package_iA32/packaging_iA32.h"
#include <stdlib.h>

void TestGet_display_pose(CuTest* tc) {
    set_display_pose(0);
    CuAssertIntEquals(tc, 0, get_display_pose());
}

CuSuite* StrUtilGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestGet_display_pose);
    return suite;
}
