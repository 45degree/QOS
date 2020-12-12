#include <gtest/gtest.h>
#include "bitmap.h"
#include "type.h"

TEST(bitmap_test, bitmap_marco_test) {
#ifdef CONFIG_32
    ASSERT_EQ(BITS_TO_ARRAY(32), 1);
    ASSERT_EQ(BITS_TO_ARRAY(33), 2);
#else
    ASSERT_EQ(BITS_TO_ARRAY(32), 1);
    ASSERT_EQ(BITS_TO_ARRAY(33), 1);
#endif
}

TEST(bitmap_test, bitmap_declear_test1) {
    DECLARE_BITMAP(test_map, 64);
#ifdef CONFIG_32
    ASSERT_EQ(sizeof(test_map) / sizeof(unsigned long), 2);
#else
    ASSERT_EQ(sizeof(test_map) / sizeof(unsigned long), 1);
#endif
}

TEST(bitmap_test, bitmap_setbits_test1) {
    DECLARE_BITMAP(test_map, 128);
#ifdef CONFIG_32
    set_bits(test_map, 0);
    EXPECT_EQ(test_map[0], 0x01);
#else
#endif
}

TEST(bitmap_test, bitmap_setbits_test2) {
    DECLARE_BITMAP(test_map, 128);
#ifdef CONFIG_32
    set_bits(test_map, 0);
    clear_bits(test_map, 0);
    EXPECT_EQ(test_map[0], 0x00);
#else
#endif
}
