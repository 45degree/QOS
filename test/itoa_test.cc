#include <memory.h>

#include <stdio.h>

/* void itoa_hex_test1(CuTest* tc) { */
/*     char temp[256]; */
/*     itoa(temp, 12, 16); */
/*     CuAssertStrEquals(tc, "0xC", temp); */

/*     itoa(temp, 16, 16); */
/*     CuAssertStrEquals(tc, "0x10", temp); */

/*     itoa(temp, -1, 16); */
/*     CuAssertStrEquals(tc, "0xFFFFFFFF", temp); */

/*     itoa(temp, 0, 16); */
/*     CuAssertStrEquals(tc, "0x0", temp); */
/* } */

/* void itoa_dec_test1(CuTest* tc) { */
/*     char temp[256]; */

/*     itoa(temp, 1, 10); */
/*     CuAssertStrEquals(tc, "1", temp); */

/*     itoa(temp, 10, 10); */
/*     CuAssertStrEquals(tc, "10", temp); */

/*     itoa(temp, 123, 10); */
/*     CuAssertStrEquals(tc, "123", temp); */

/*     itoa(temp, 0, 10); */
/*     CuAssertStrEquals(tc, "0", temp); */

/*     itoa(temp, -1, 10); */
/*     CuAssertStrEquals(tc, "-1", temp); */

/*     itoa(temp, -10, 10); */
/*     CuAssertStrEquals(tc, "-10", temp); */

/*     itoa(temp, -232, 10); */
/*     CuAssertStrEquals(tc, "-232", temp); */
/* } */

/* CuSuite* itoa_test() { */
/*     CuSuite* suite = CuSuiteNew(); */
/*     SUITE_ADD_TEST(suite, itoa_hex_test1); */
/*     SUITE_ADD_TEST(suite, itoa_dec_test1); */
/*     return suite; */
/* } */
