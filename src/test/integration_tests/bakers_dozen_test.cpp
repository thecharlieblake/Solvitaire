//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "../test_helper.h"

typedef test_helper th;

const static std::string path = "resources/bakers_dozen/";
const static std::string type = "simple-bakers-dozen";

TEST(BakersDozen, SimpleSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "SimpleSolvable.json", type));
}

TEST(BakersDozen, ComplexSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "ComplexSolvable.json", type));
}

TEST(BakersDozen, SimpleUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "SimpleUnsolvable.json", type));
}

TEST(BakersDozen, ComplexUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "ComplexUnsolvable.json", type));
}
