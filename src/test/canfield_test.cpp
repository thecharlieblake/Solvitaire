//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "test_helper.h"

typedef test_helper th;

const static std::string path = "resources/canfield/";
const static std::string type = "simple-canfield";

TEST(Canfield, SimpleSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "SimpleSolvable.json", type));
}

TEST(Canfield, ComplexSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "ComplexSolvable.json", type));
}

TEST(Canfield, SimpleUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "SimpleUnsolvable.json", type));
}

TEST(Canfield, ComplexUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "ComplexUnsolvable.json", type));
}
