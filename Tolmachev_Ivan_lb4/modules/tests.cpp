#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace std;

string kmp_search(const string& P, const string& T) {
    if (P.empty()) return "-1";

    int m = P.size();
    vector<int> prefix(m, 0);
    for (int i = 1; i < m; ++i) {
        int j = prefix[i - 1];
        while (j > 0 && P[i] != P[j])
            j = prefix[j - 1];
        if (P[i] == P[j])
            ++j;
        prefix[i] = j;
    }

    vector<int> positions;
    int n = T.size();
    int j = 0;
    for (int i = 0; i < n; ++i) {
        while (j > 0 && T[i] != P[j])
            j = prefix[j - 1];
        if (T[i] == P[j])
            ++j;
        if (j == m) {
            positions.push_back(i - m + 1);
            j = prefix[j - 1];
        }
    }

    if (positions.empty()) return "-1";
    string result;
    for (size_t k = 0; k < positions.size(); ++k) {
        if (k > 0) result += ',';
        result += to_string(positions[k]);
    }
    return result;
}

int cyclic_shift(const string& A, const string& B) {
    int n = A.size(), m = B.size();
    if (n != m) return -1;
    if (n == 0) return 0;

    vector<int> prefix(m, 0);
    for (int i = 1; i < m; ++i) {
        int j = prefix[i - 1];
        while (j > 0 && B[i] != B[j])
            j = prefix[j - 1];
        if (B[i] == B[j])
            ++j;
        prefix[i] = j;
    }

    int j = 0;
    int total = 2 * n;
    for (int i = 0; i < total; ++i) {
        char cur = A[i % n];
        while (j > 0 && cur != B[j])
            j = prefix[j - 1];
        if (cur == B[j])
            ++j;
        if (j == m) {
            int start = i - m + 1;
            if (start < n)
                return start;
            j = prefix[j - 1];
        }
    }
    return -1;
}

TEST(KMPSearchTest, EmptyPattern) {
    EXPECT_EQ(kmp_search("", "abc"), "-1");
}

TEST(KMPSearchTest, PatternNotFound) {
    EXPECT_EQ(kmp_search("abc", "defgh"), "-1");
}

TEST(KMPSearchTest, SingleMatch) {
    EXPECT_EQ(kmp_search("abc", "xyzabc123"), "3");
}

TEST(KMPSearchTest, MultipleMatches) {
    EXPECT_EQ(kmp_search("aa", "aaa"), "0,1");
}

TEST(KMPSearchTest, PatternEqualsText) {
    EXPECT_EQ(kmp_search("abc", "abc"), "0");
}

TEST(KMPSearchTest, PatternLongerThanText) {
    EXPECT_EQ(kmp_search("abcdef", "abc"), "-1");
}

TEST(KMPSearchTest, OverlapprefixngMatches) {
    EXPECT_EQ(kmp_search("aba", "ababa"), "0,2");
}

TEST(CyclicShiftTest, DifferentLength) {
    EXPECT_EQ(cyclic_shift("abc", "ab"), -1);
}

TEST(CyclicShiftTest, EmptyStrings) {
    EXPECT_EQ(cyclic_shift("", ""), 0);
}

TEST(CyclicShiftTest, IdenticalStrings) {
    EXPECT_EQ(cyclic_shift("abc", "abc"), 0);
}

TEST(CyclicShiftTest, NormalShift) {
    EXPECT_EQ(cyclic_shift("defabc", "abcdef"), 3);
}

TEST(CyclicShiftTest, NotAShift) {
    EXPECT_EQ(cyclic_shift("abc", "abd"), -1);
}

TEST(CyclicShiftTest, AllSameCharacters) {
    EXPECT_EQ(cyclic_shift("aaaa", "aaaa"), 0);
}

TEST(CyclicShiftTest, ShiftByOne) {
    EXPECT_EQ(cyclic_shift("ab", "ba"), 1);
}

TEST(CyclicShiftTest, ShiftBySeveral) {
    EXPECT_EQ(cyclic_shift("eabcd", "abcde"), 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}