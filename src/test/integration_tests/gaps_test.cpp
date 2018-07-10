//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "../test_helper.h"

typedef test_helper th;

const static std::string path = "resources/gaps/";
const static std::string type = "simple-gaps";

TEST(AlphaStar, SimpleSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "SimpleSolvable.json", type));
}

TEST(AlphaStar, ComplexSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "ComplexSolvable.json", type));
}

TEST(AlphaStar, SimpleUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "SimpleUnsolvable.json", type));
}

TEST(AlphaStar, ComplexUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "ComplexUnsolvable.json", type));
}
