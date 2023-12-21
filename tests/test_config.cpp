//
// Created by guozr on 23-12-21.
//

#include <gtest/gtest.h>

TEST(Test_Config, BasicAssertions) {
// Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world1");
// Expect equality.
    EXPECT_EQ(7 * 6, 42);
}