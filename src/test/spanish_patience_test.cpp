//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "test_helper.h"

typedef test_helper th;

const static std::string type = "simple-spanish-patience";

TEST(SpanishPatience, SPTestName) {
    EXPECT_TRUE(th::is_solvable("SPTestName", type));
}
