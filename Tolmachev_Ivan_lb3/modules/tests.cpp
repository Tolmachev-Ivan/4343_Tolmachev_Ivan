#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm>

std::string edit_operations(int base_replace, int base_insert, int base_delete,
                            char special_replace_char, int special_replace_cost,
                            char special_insert_char, int special_insert_cost,
                            const std::string& A, const std::string& B) {
    int n = A.size();
    int m = B.size();

    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1));

    for (int i = 0; i <= n; ++i) {
        dp[i][0] = i * base_delete;
    }
    for (int j = 1; j <= m; ++j) {
        char c = B[j - 1];
        int ins_cost = (c == special_insert_char) ? special_insert_cost : base_insert;
        dp[0][j] = dp[0][j - 1] + ins_cost;
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (A[i - 1] == B[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                char a_char = A[i - 1];
                char b_char = B[j - 1];

                int rep_cost = (a_char == special_replace_char) ? special_replace_cost : base_replace;
                int ins_cost = (b_char == special_insert_char) ? special_insert_cost : base_insert;
                int del_cost = base_delete;

                int replace = dp[i - 1][j - 1] + rep_cost;
                int insert  = dp[i][j - 1] + ins_cost;
                int del     = dp[i - 1][j] + del_cost;

                dp[i][j] = std::min({replace, insert, del});
            }
        }
    }

    std::string ops;
    int i = n, j = m;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && A[i - 1] == B[j - 1]) {
            ops.push_back('M');
            --i; --j;
        } else {
            char a_char = (i > 0) ? A[i - 1] : '\0';
            char b_char = (j > 0) ? B[j - 1] : '\0';

            int rep_cost = (a_char == special_replace_char) ? special_replace_cost : base_replace;
            int ins_cost = (b_char == special_insert_char) ? special_insert_cost : base_insert;
            int del_cost = base_delete;

            if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + rep_cost) {
                ops.push_back('R');
                --i; --j;
            } else if (j > 0 && dp[i][j] == dp[i][j - 1] + ins_cost) {
                ops.push_back('I');
                --j;
            } else if (i > 0 && dp[i][j] == dp[i - 1][j] + del_cost) {
                ops.push_back('D');
                --i;
            } else {
                if (i > 0 && j > 0) {
                    ops.push_back('R'); --i; --j;
                } else if (j > 0) {
                    ops.push_back('I'); --j;
                } else {
                    ops.push_back('D'); --i;
                }
            }
        }
    }
    std::reverse(ops.begin(), ops.end());
    return ops;
}


TEST(EditDistanceTest, EmptyStrings) {
    EXPECT_EQ(edit_operations(2, 3, 4, 'x', 99, 'y', 99, "", ""), "");
}

TEST(EditDistanceTest, OnlyInsertions) {
    EXPECT_EQ(edit_operations(1, 5, 1, 'x', 99, 'y', 99, "", "abc"), "III");
    auto ops = edit_operations(1, 1, 1, 'x', 9, 'z', 100, "", "a");
    EXPECT_EQ(ops, "I");
}

TEST(EditDistanceTest, OnlyDeletions) {
    EXPECT_EQ(edit_operations(1, 1, 5, 'x', 99, 'y', 99, "abc", ""), "DDD");
}

TEST(EditDistanceTest, Identical) {
    EXPECT_EQ(edit_operations(2, 3, 4, 'x', 99, 'y', 99, "hello", "hello"), "MMMMM");
}

TEST(EditDistanceTest, SimpleReplace) {
    EXPECT_EQ(edit_operations(2, 10, 10, 'x', 99, 'y', 99, "a", "b"), "R");
    EXPECT_EQ(edit_operations(10, 2, 2, 'x', 99, 'y', 99, "a", "b"), "DI");
}

TEST(EditDistanceTest, SpecialReplace) {
    EXPECT_EQ(edit_operations(10, 5, 5, 'a', 1, 'y', 99, "a", "b"), "R");
    EXPECT_EQ(edit_operations(10, 5, 5, 'a', 100, 'y', 99, "a", "b"), "DI");
}

TEST(EditDistanceTest, SpecialInsert) {
    EXPECT_EQ(edit_operations(5, 10, 5, 'x', 99, 'b', 1, "a", "b"), "R");
    EXPECT_EQ(edit_operations(20, 10, 5, 'x', 99, 'b', 1, "a", "b"), "DI");
}

TEST(EditDistanceTest, PreferMatchOverAnyOperation) {
    EXPECT_EQ(edit_operations(1, 1, 1, 'x', 99, 'y', 99, "abc", "adc"), "MRM");
}

TEST(EditDistanceTest, MultipleEqualCostPaths) {
    EXPECT_EQ(edit_operations(1, 1, 1, 'x', 99, 'y', 99, "ab", "ba"), "RR");
}

TEST(EditDistanceTest, OneSpecialCharAndBasePrices) {
    EXPECT_EQ(edit_operations(10, 10, 10, 'z', 0, 'y', 99, "z", "w"), "R");
    EXPECT_EQ(edit_operations(10, 10, 10, 'z', 0, 'y', 99, "a", "w"), "R");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}