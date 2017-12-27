//
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "test_helper.h"

typedef test_helper th;

const static std::string path = "resources/free_cell/";
const static std::string type = "simple-free-cell";

TEST(FreeCell, SimpleSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "SimpleSolvable.json", type));
}

TEST(FreeCell, ComplexSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "ComplexSolvable.json", type));
}

TEST(FreeCell, SimpleUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "SimpleUnsolvable.json", type));
}

TEST(FreeCell, ComplexUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "ComplexUnsolvable.json", type));
}
