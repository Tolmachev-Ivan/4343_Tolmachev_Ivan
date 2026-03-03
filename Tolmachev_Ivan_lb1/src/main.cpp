#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_set>

using namespace std;

int N;
int best;

unsigned long long board_mask[40];
vector<tuple<int,int,int>> cur, ans;

int total_cells;
int filled_cells = 0;

bool find_empty(int &x, int &y)
{
    for (x = 0; x < N; x++)
    {
        if (board_mask[x] != ((1ULL << N) - 1))
        {
            for (y = 0; y < N; y++)
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
    if (x + size > N || y + size > N) return false;

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

        cout << "\n====== Предварительный лучший результат ======\n";
        cout << "Количесвто квадратов: " << best << endl;
        cout << "x y размер\n";
        for (auto [xc, yc, sizec] : ans)
            cout << xc << " " << yc << "   " << sizec << "\n";

        return;
    }

    int remaining = total_cells - filled;

    int x, y;
    if (!find_empty(x, y))
        return;

    int max_possible = min(N - x, N - y);
    if (max_possible <= 0) return;

    int min_additional = (remaining + max_possible * max_possible - 1)
                         / (max_possible * max_possible);

    if (used + min_additional >= best) return;

    int max_s = min(N - x, N - y);
    max_s = min(N - 1, max_s);

    while (max_s > 0 && !can_place(x, y, max_s))
        max_s--;

    unordered_set<int> tried_sizes;

    for (int size = max_s; size >= 1; size--)
    {
        if (!can_place(x, y, size)) continue;
        if (tried_sizes.find(size) != tried_sizes.end()) continue;

        place(x, y, size);
        cur.emplace_back(x + 1, y + 1, size);

        cout << "\nРазмещение квадрата\n";
        cout << "x = " << x;
        cout << "; y = " << y;
        cout << "; размер = " << size << endl;

        backtrack(used + 1);

        cout << "\nУдаление квадрата\n";
        cout << "x = " << x;
        cout << "; y = " << y;
        cout << "; размер = " << size << endl;

        cur.pop_back();
        remove_sq(x, y, size);

        tried_sizes.insert(size);
    }
}

int main()
{
    cin >> N;

    total_cells = N * N;
    best = N * 2;

    for (int i = 0; i < 40; i++)
        board_mask[i] = 0;

    if (N % 2 == 0)
    {
        int k = N / 2;
        cout << 4 << "\n";
        cout << 1 << " " << 1 << " " << k << "\n";
        cout << 1 << " " << k + 1 << " " << k << "\n";
        cout << k + 1 << " " << 1 << " " << k << "\n";
        cout << k + 1 << " " << k + 1 << " " << k << "\n";
        return 0;
    }

    if (N % 3 == 0)
    {
        int k = N / 3;
        cout << 6 << "\n";
        cout << 1 << " " << 1 << " " << 2 * k << "\n";
        cout << 1 << " " << 2 * k + 1 << " " << k << "\n";
        cout << k + 1 << " " << 2 * k + 1 << " " << k << "\n";
        cout << 2 * k + 1 << " " << 1 << " " << k << "\n";
        cout << 2 * k + 1 << " " << k + 1 << " " << k << "\n";
        cout << 2 * k + 1 << " " << 2 * k + 1 << " " << k << "\n";
        return 0;
    }

    if (N % 5 == 0) 
    { 
        int k = N / 5 * 3; 
        cout << 8 << "\n"; 
        cout << 1 << " " << 1 << " " << k << "\n"; 
        k = N / 5 * 2; 
        cout << 1 << " " << k * 1.5 + 1 << " " << k << "\n"; 
        cout << k + 1 << " " << k * 3 / 2 + 1 << " " << k << "\n"; 
        cout << k * 1.5 + 1 << " " << 1 << " " << k << "\n"; 
        k = N / 5; 
        cout << k * 3 + 1 << " " << k * 2 + 1 << " " << k << "\n"; 
        cout << k * 4 + 1 << " " << k * 2 + 1 << " " << k << "\n"; 
        cout << k * 4 + 1 << " " << k * 3 + 1 << " " << k << "\n"; 
        cout << k * 4 + 1 << " " << k * 4 + 1 << " " << k << "\n"; 
        return 0; 
    }

   int s1 = N / 2 + 1;
   int s2 = N / 2;

   place(0, 0, s1);
   cur.emplace_back(1, 1, s1);

   place(0, s1, s2);
   cur.emplace_back(1, s1 + 1, s2);

   place(s1, 0, s2);
   cur.emplace_back(s1 + 1, 1, s2);

    backtrack(3);

    cout << best << "\n";
    for (auto [x, y, size] : ans)
        cout << x << " " << y << " " << size << "\n";

    return 0;
}