//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

const int actualValTrue  = 4;
const int actualValFalse = 8;
const int expectVal      = 4;

namespace {
    TEST(StrCompare, CStrEqual) {
        EXPECT_EQ(expectVal, actualValTrue);
    }

    TEST(StrCompare, CStrNotEqual) {
        EXPECT_NE(expectVal, actualValFalse);
    }
}
