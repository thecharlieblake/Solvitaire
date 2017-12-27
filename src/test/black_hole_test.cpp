//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "test_helper.h"

typedef test_helper th;

const static std::string path = "resources/black_hole/";
const static std::string type = "simple-black-hole";

TEST(BlackHole, SimpleSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "SimpleSolvable.json", type));
}

TEST(BlackHole, ComplexSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "ComplexSolvable.json", type));
}

TEST(BlackHole, SimpleUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "SimpleUnsolvable.json", type));
}

TEST(BlackHole, ComplexUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "ComplexUnsolvable.json", type));
}
