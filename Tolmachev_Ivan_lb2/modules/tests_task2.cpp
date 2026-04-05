#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cmath>
#include <gtest/gtest.h>

using namespace std;

const double INF = 1e100;
const double EPS = 1e-9;

double best_cost = INF;
vector<int> best_path;

double reduceMatrix(vector<vector<double>>& mat,
                    const vector<int>& rows,
                    const vector<int>& cols) {
    int n = rows.size();
    int m = cols.size();
    double total = 0.0;

    for (int i = 0; i < n; ++i) {
        double min_val = INF;
        for (int j = 0; j < m; ++j) {
            if (mat[i][j] < min_val) min_val = mat[i][j];
        }
        if (min_val > EPS && min_val < INF / 2) {
            total += min_val;
            for (int j = 0; j < m; ++j) {
                if (mat[i][j] < INF / 2) mat[i][j] -= min_val;
            }
        }
    }

    for (int j = 0; j < m; ++j) {
        double min_val = INF;
        for (int i = 0; i < n; ++i) {
            if (mat[i][j] < min_val) min_val = mat[i][j];
        }
        if (min_val > EPS && min_val < INF / 2) {
            total += min_val;
            for (int i = 0; i < n; ++i) {
                if (mat[i][j] < INF / 2) mat[i][j] -= min_val;
            }
        }
    }
    return total;
}

void branchAndBound(vector<vector<double>> mat,
                    vector<int> rows,
                    vector<int> cols,
                    double lower_bound,
                    vector<int>& next_arc,
                    vector<int>& prev_arc,
                    int included_count,
                    int n) {
    int k = rows.size();
    int l = cols.size();

    if (lower_bound >= best_cost - EPS) return;

    if (k == 1 && l == 1) {
        int u = rows[0];
        int v = cols[0];
        if (next_arc[u] != -1 || prev_arc[v] != -1) return;
        next_arc[u] = v;
        prev_arc[v] = u;

        vector<int> path;
        int cur = 0;
        do {
            path.push_back(cur);
            cur = next_arc[cur];
        } while (cur != 0);

        double cost = lower_bound;
        if (cost < best_cost - EPS) {
            best_cost = cost;
            best_path = path;
        }

        next_arc[u] = -1;
        prev_arc[v] = -1;
        return;
    }

    int best_i = -1, best_j = -1;
    double best_delta = -1.0;
    bool force_left = false;

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < l; ++j) {
            if (fabs(mat[i][j]) < EPS) {
                double row_min = INF;
                for (int jj = 0; jj < l; ++jj) {
                    if (jj != j && mat[i][jj] < row_min) row_min = mat[i][jj];
                }
                double col_min = INF;
                for (int ii = 0; ii < k; ++ii) {
                    if (ii != i && mat[ii][j] < col_min) col_min = mat[ii][j];
                }
                double delta = row_min + col_min;
                if (delta >= INF / 2) {
                    force_left = true;
                    best_i = i; best_j = j;
                    best_delta = INF;
                    break;
                }
                if (delta > best_delta + EPS) {
                    best_delta = delta;
                    best_i = i; best_j = j;
                }
            }
        }
        if (force_left) break;
    }

    if (best_i == -1) return;

    int u = rows[best_i];
    int v = cols[best_j];

    if (!force_left) {
        vector<vector<double>> mat_right = mat;
        mat_right[best_i][best_j] = INF;
        double add = reduceMatrix(mat_right, rows, cols);
        double new_bound = lower_bound + add;
        if (new_bound < best_cost - EPS) {
            branchAndBound(mat_right, rows, cols, new_bound,
                           next_arc, prev_arc, included_count, n);
        }
    }

    if (next_arc[u] != -1 || prev_arc[v] != -1) return;

    if (included_count < n - 1) {
        int x = v;
        while (x != -1) {
            if (x == u) return;
            x = next_arc[x];
        }
    }

    vector<int> new_rows, new_cols;
    for (int i = 0; i < k; ++i) if (i != best_i) new_rows.push_back(rows[i]);
    for (int j = 0; j < l; ++j) if (j != best_j) new_cols.push_back(cols[j]);

    int new_k = new_rows.size();
    int new_l = new_cols.size();
    vector<vector<double>> mat_left(new_k, vector<double>(new_l));

    for (int i = 0, ni = 0; i < k; ++i) {
        if (i == best_i) continue;
        for (int j = 0, nj = 0; j < l; ++j) {
            if (j == best_j) continue;
            mat_left[ni][nj] = mat[i][j];
            ++nj;
        }
        ++ni;
    }

    int pos_v = -1, pos_u = -1;
    for (int i = 0; i < new_k; ++i) if (new_rows[i] == v) { pos_v = i; break; }
    for (int j = 0; j < new_l; ++j) if (new_cols[j] == u) { pos_u = j; break; }
    if (pos_v != -1 && pos_u != -1) {
        mat_left[pos_v][pos_u] = INF;
    }

    next_arc[u] = v;
    prev_arc[v] = u;

    double add = reduceMatrix(mat_left, new_rows, new_cols);
    double new_bound = lower_bound + add;

    if (new_bound < best_cost - EPS) {
        branchAndBound(mat_left, new_rows, new_cols, new_bound,
                       next_arc, prev_arc, included_count + 1, n);
    }

    next_arc[u] = -1;
    prev_arc[v] = -1;
}

struct LittleResult {
    bool found;
    double cost;
    vector<int> path;
};

LittleResult littleTsp(const vector<vector<double>>& w) {
    int n = w.size();
    best_cost = INF;
    best_path.clear();

    vector<int> rows(n), cols(n);
    for (int i = 0; i < n; ++i) rows[i] = cols[i] = i;

    vector<vector<double>> mat = w;
    double lower_bound = reduceMatrix(mat, rows, cols);

    vector<int> next_arc(n, -1), prev_arc(n, -1);

    branchAndBound(mat, rows, cols, lower_bound, next_arc, prev_arc, 0, n);

    if (best_path.empty())
        return {false, 0.0, {}};
    else
        return {true, best_cost, best_path};
}

TEST(LittleTest, SampleInput) {
    vector<vector<double>> w = {
        {INF, 1.0, 3.0},
        {3.0, INF, 1.0},
        {1.0, 2.0, INF}
    };
    auto res = littleTsp(w);
    EXPECT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 3.0);
    vector<int> expected = {0, 1, 2};
    EXPECT_EQ(res.path, expected);
}

TEST(LittleTest, TwoVertices) {
    vector<vector<double>> w = {
        {INF, 5.0},
        {3.0, INF}
    };
    auto res = littleTsp(w);
    EXPECT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 8.0);
    vector<int> expected = {0, 1};
    EXPECT_EQ(res.path, expected);
}

TEST(LittleTest, NonSymmetricThreeVertices) {
    vector<vector<double>> w = {
        {INF, 1.0, 2.0},
        {10.0, INF, 3.0},
        {20.0, 30.0, INF}
    };
    auto res = littleTsp(w);
    EXPECT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 24.0);
    vector<int> expected = {0, 1, 2};
    EXPECT_EQ(res.path, expected);
}

TEST(LittleTest, ZeroWeights) {
    int n = 3;
    vector<vector<double>> w(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; ++i) w[i][i] = INF;
    auto res = littleTsp(w);
    EXPECT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 0.0);
    EXPECT_EQ(res.path.size(), 3);
    EXPECT_EQ(res.path[0], 0);
    vector<bool> seen(n, false);
    for (int v : res.path) {
        EXPECT_FALSE(seen[v]);
        seen[v] = true;
    }
    for (int i = 0; i < n; ++i) EXPECT_TRUE(seen[i]);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}