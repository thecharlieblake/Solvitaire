//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "test_helper.h"

TEST(SpanishPatience, SPTestName) {
    EXPECT_TRUE(test_helper::is_solvable("i", "simple-spanish-patience"));
}
