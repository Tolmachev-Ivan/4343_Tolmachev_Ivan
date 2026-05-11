#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

const int ALPH_SIZE = 5;

int char_to_idx(char c) {
    switch (c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        case 'N': return 4;
    }
    return -1;
}

struct Vertex1 {
    int next[ALPH_SIZE];
    int fail;
    int out;
    vector<int> patterns;
    int pattern_len;

    Vertex1() {
        fill(next, next + ALPH_SIZE, -1);
        fail = 0;
        out = -1;
        pattern_len = 0;
    }
};

vector<pair<int, int>> aho_corasick_multi(const string& T, const vector<string>& patterns) {
    vector<Vertex1> trie(1);
    vector<int> pat_len(patterns.size() + 1);
    for (size_t i = 0; i < patterns.size(); ++i) {
        const string& s = patterns[i];
        int v = 0;
        for (char c : s) {
            int idx = char_to_idx(c);
            if (trie[v].next[idx] == -1) {
                trie[v].next[idx] = trie.size();
                trie.emplace_back();
            }
            v = trie[v].next[idx];
        }
        trie[v].patterns.push_back(i + 1);
        trie[v].pattern_len = s.length();
        pat_len[i + 1] = s.length();
    }

    queue<int> q;
    trie[0].fail = 0;
    for (int c = 0; c < ALPH_SIZE; ++c) {
        if (trie[0].next[c] != -1) {
            int v = trie[0].next[c];
            trie[v].fail = 0;
            q.push(v);
        } else {
            trie[0].next[c] = 0;
        }
    }
    while (!q.empty()) {
        int v = q.front(); q.pop();
        if (!trie[trie[v].fail].patterns.empty()) {
            trie[v].out = trie[v].fail;
        } else {
            trie[v].out = trie[trie[v].fail].out;
        }
        for (int c = 0; c < ALPH_SIZE; ++c) {
            int u = trie[v].next[c];
            if (u != -1) {
                trie[u].fail = trie[trie[v].fail].next[c];
                q.push(u);
            } else {
                trie[v].next[c] = trie[trie[v].fail].next[c];
            }
        }
    }

    vector<vector<int>> occ_by_start(T.length() + 2);
    int current = 0;
    for (int i = 0; i < (int)T.length(); ++i) {
        int idx = char_to_idx(T[i]);
        current = trie[current].next[idx];
        int temp = current;
        while (temp != 0) {
            if (!trie[temp].patterns.empty()) {
                int start = i - trie[temp].pattern_len + 2;
                for (int p : trie[temp].patterns) {
                    occ_by_start[start].push_back(p);
                }
            }
            temp = trie[temp].out;
            if (temp == -1) break;
        }
    }

    vector<pair<int, int>> result;
    for (int pos = 1; pos <= (int)T.length(); ++pos) {
        if (!occ_by_start[pos].empty()) {
            sort(occ_by_start[pos].begin(), occ_by_start[pos].end());
            for (int p : occ_by_start[pos]) {
                result.emplace_back(pos, p);
            }
        }
    }
    return result;
}

struct Vertex2 {
    int next[ALPH_SIZE];
    int fail;
    int out;
    vector<int> offsets;
    int depth;

    Vertex2() {
        fill(next, next + ALPH_SIZE, -1);
        fail = 0;
        out = -1;
        depth = 0;
    }
};

vector<int> aho_corasick_wildcard(const string& T, const string& P, char wildcard) {
    vector<pair<string, int>> fragments;
    string current;
    int lenP = P.length();
    for (int i = 0; i < lenP; ++i) {
        if (P[i] == wildcard) {
            if (!current.empty()) {
                fragments.emplace_back(current, i - (int)current.length());
                current.clear();
            }
        } else {
            current += P[i];
        }
    }
    if (!current.empty()) {
        fragments.emplace_back(current, lenP - (int)current.length());
    }

    int frag_count = fragments.size();
    if (frag_count == 0) return {};

    vector<Vertex2> trie(1);
    for (const auto& f : fragments) {
        const string& s = f.first;
        int offset = f.second;
        int v = 0;
        for (char c : s) {
            int idx = char_to_idx(c);
            if (trie[v].next[idx] == -1) {
                trie[v].next[idx] = trie.size();
                trie.emplace_back();
                trie.back().depth = trie[v].depth + 1;
            }
            v = trie[v].next[idx];
        }
        trie[v].offsets.push_back(offset);
    }

    queue<int> q;
    trie[0].fail = 0;
    for (int c = 0; c < ALPH_SIZE; ++c) {
        if (trie[0].next[c] != -1) {
            int v = trie[0].next[c];
            trie[v].fail = 0;
            q.push(v);
        } else {
            trie[0].next[c] = 0;
        }
    }
    while (!q.empty()) {
        int v = q.front(); q.pop();
        if (!trie[trie[v].fail].offsets.empty()) {
            trie[v].out = trie[v].fail;
        } else {
            trie[v].out = trie[trie[v].fail].out;
        }
        for (int c = 0; c < ALPH_SIZE; ++c) {
            int u = trie[v].next[c];
            if (u != -1) {
                trie[u].fail = trie[trie[v].fail].next[c];
                q.push(u);
            } else {
                trie[v].next[c] = trie[trie[v].fail].next[c];
            }
        }
    }

    vector<int> C(T.length() + 2, 0);
    int current_state = 0;
    for (int i = 0; i < (int)T.length(); ++i) {
        int idx = char_to_idx(T[i]);
        current_state = trie[current_state].next[idx];
        int temp = current_state;
        while (temp != 0) {
            if (!trie[temp].offsets.empty()) {
                for (int off : trie[temp].offsets) {
                    int start = i - trie[temp].depth - off + 2;
                    if (start >= 1 && start <= (int)T.length() - lenP + 1) {
                        C[start]++;
                    }
                }
            }
            temp = trie[temp].out;
            if (temp == -1) break;
        }
    }

    vector<int> result;
    for (int i = 1; i <= (int)T.length(); ++i) {
        if (C[i] == frag_count) {
            result.push_back(i);
        }
    }
    return result;
}

TEST(AhoCorasickMulti, Basic) {
    auto res = aho_corasick_multi("ACTG", {"AC", "TG"});
    vector<pair<int, int>> expected = {{1, 1}, {3, 2}};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickMulti, SinglePatternFullText) {
    auto res = aho_corasick_multi("ACTG", {"ACTG"});
    vector<pair<int, int>> expected = {{1, 1}};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickMulti, NoMatch) {
    auto res = aho_corasick_multi("GG", {"A", "C"});
    EXPECT_TRUE(res.empty());
}

TEST(AhoCorasickMulti, OverlappingPatterns) {
    auto res = aho_corasick_multi("AAA", {"AA", "A"});
    vector<pair<int, int>> expected = {
        {1, 1}, {1, 2}, {2, 1}, {2, 2}, {3, 2}
    };
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickMulti, PatternN) {
    auto res = aho_corasick_multi("N", {"N"});
    vector<pair<int, int>> expected = {{1, 1}};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickMulti, MultiplePatternsSame) {
    auto res = aho_corasick_multi("AC", {"A", "A"});
    vector<pair<int, int>> expected = {{1, 1}, {1, 2}};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickWildcard, Basic) {
    auto res = aho_corasick_wildcard("ACTANCA", "A$$A$", '$');
    vector<int> expected = {1};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickWildcard, WildcardAtStart) {
    auto res = aho_corasick_wildcard("AAC", "?AC", '?');
    vector<int> expected = {1};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickWildcard, WildcardAtEnd) {
    auto res = aho_corasick_wildcard("ACT", "AC?", '?');
    vector<int> expected = {1};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickWildcard, MultipleWildcards) {
    auto res = aho_corasick_wildcard("ACTGA", "A???A", '?');
    vector<int> expected = {1};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickWildcard, NoMatch) {
    auto res = aho_corasick_wildcard("GGG", "A?C", '?');
    EXPECT_TRUE(res.empty());
}

TEST(AhoCorasickWildcard, SingleWildcard) {
    auto res = aho_corasick_wildcard("AN", "A?", '?');
    vector<int> expected = {1};
    EXPECT_EQ(res, expected);
}

TEST(AhoCorasickWildcard, OnlyFragmentsNoWildcards) {
    auto res = aho_corasick_wildcard("ACTG", "CT", '*');
    vector<int> expected = {2};
    EXPECT_EQ(res, expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}