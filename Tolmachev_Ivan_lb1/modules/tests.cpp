#include <gtest/gtest.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_set>

using namespace std;

int N_global;
int best;
unsigned long long board_mask[40];
vector<tuple<int,int,int>> cur, ans;
int total_cells;
int filled_cells = 0;

bool find_empty(int &x, int &y)
{
    for (x = 0; x < N_global; x++)
    {
        if (board_mask[x] != ((1ULL << N_global) - 1))
        {
            for (y = 0; y < N_global; y++)
                if (!(board_mask[x] & (1ULL << y)))
                    return true;
        }
    }
    return false;
}

int count_filled()
{
    return filled_cells;
}

bool can_place(int x, int y, int size)
{
    if (x + size > N_global || y + size > N_global) return false;

    unsigned long long mask = ((1ULL << size) - 1) << y;

    for (int i = x; i < x + size; i++)
        if (board_mask[i] & mask)
            return false;

    return true;
}

void place(int x, int y, int size)
{
    unsigned long long mask = ((1ULL << size) - 1) << y;
    for (int i = x; i < x + size; i++)
        board_mask[i] |= mask;
    filled_cells += size * size;
}

void remove_sq(int x, int y, int size)
{
    unsigned long long mask = ((1ULL << size) - 1) << y;
    for (int i = x; i < x + size; i++)
        board_mask[i] &= ~mask;
    filled_cells -= size * size;
}

void backtrack(int used)
{
    if (used >= best) return;

    int filled = count_filled();
    if (filled == total_cells)
    {
        best = used;
        ans = cur;
        return;
    }

    int remaining = total_cells - filled;

    int x, y;
    if (!find_empty(x, y))
        return;

    int max_possible = min(N_global - x, N_global - y);
    if (max_possible <= 0) return;

    int min_additional = (remaining + max_possible * max_possible - 1)
                         / (max_possible * max_possible);

    if (used + min_additional >= best) return;

    int max_s = min(N_global - x, N_global - y);
    max_s = min(N_global - 1, max_s);

    while (max_s > 0 && !can_place(x, y, max_s))
        max_s--;

    unordered_set<int> tried_sizes;

    for (int size = max_s; size >= 1; size--)
    {
        if (!can_place(x, y, size)) continue;
        if (tried_sizes.find(size) != tried_sizes.end()) continue;

        place(x, y, size);
        cur.emplace_back(x + 1, y + 1, size);

        backtrack(used + 1);

        cur.pop_back();
        remove_sq(x, y, size);

        tried_sizes.insert(size);
    }
}

int solve_for_n(int n)
{
    N_global = n;
    total_cells = N_global * N_global;
    best = N_global * 2;
    filled_cells = 0;
    cur.clear();
    ans.clear();
    for (int i = 0; i < 40; ++i) board_mask[i] = 0;

    if (N_global % 2 == 0) return 4;
    if (N_global % 3 == 0) return 6;
    if (N_global % 5 == 0) return 8;

    int s1 = N_global / 2 + 1;
    int s2 = N_global / 2;

    place(0, 0, s1);
    cur.emplace_back(1, 1, s1);

    place(0, s1, s2);
    cur.emplace_back(1, s1 + 1, s2);

    place(s1, 0, s2);
    cur.emplace_back(s1 + 1, 1, s2);

    backtrack(3);

    return best;
}


TEST(BoundaryCases, AnswersAtLeastN)
{
    vector<int> Ns = {3, 5, 7, 11};
    for (int n : Ns)
    {
        int res = solve_for_n(n);
        EXPECT_GE(res, n);
    }
}

TEST(PrimeCases, LargePrimesHaveReasonableAnswers)
{
    vector<int> Ns = {19, 29, 37};
    for (int n : Ns)
    {
        int res = solve_for_n(n);
        EXPECT_GT(res, 0);
        EXPECT_LE(res, n * n);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}