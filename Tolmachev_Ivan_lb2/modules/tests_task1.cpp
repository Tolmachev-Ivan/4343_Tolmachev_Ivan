#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <limits>
#include <gtest/gtest.h>

using namespace std;

const double INF = 1e100;
const double EPS = 1e-12;

struct ApproxResult {
    bool found;
    double cost;
    vector<int> path;
};

ApproxResult approx_tsp(int start, const vector<vector<double>>& w) {
    int n = (int)w.size();

    vector<double> minEdge(n, INF);
    vector<int> parent(n, -1);
    vector<bool> used(n, false);

    minEdge[start] = 0.0;

    for (int it = 0; it < n; ++it) {
        int v = -1;
        for (int i = 0; i < n; ++i) {
            if (!used[i] && (v == -1 || minEdge[i] < minEdge[v])) {
                v = i;
            }
        }
        if (v == -1 || minEdge[v] >= INF / 2) {
            return {false, 0.0, {}};
        }

        used[v] = true;

        for (int to = 0; to < n; ++to) {
            if (!used[to] && w[v][to] < minEdge[to]) {
                minEdge[to] = w[v][to];
                parent[to] = v;
            }
        }
    }

    vector<vector<pair<int, double>>> tree(n);
    for (int v = 0; v < n; ++v) {
        if (v == start) continue;
        if (parent[v] == -1) return {false, 0.0, {}};
        double weight = w[v][parent[v]];
        tree[v].emplace_back(parent[v], weight);
        tree[parent[v]].emplace_back(v, weight);
    }

    for (int v = 0; v < n; ++v) {
        sort(tree[v].begin(), tree[v].end(),
             [](const pair<int,double>& a, const pair<int,double>& b) {
                 if (fabs(a.second - b.second) > EPS) return a.second < b.second;
                 return a.first < b.first;
             });
    }

    vector<int> order;
    vector<bool> visited(n, false);

    function<void(int)> dfs = [&](int v) {
        visited[v] = true;
        order.push_back(v);
        for (auto [to, weight] : tree[v]) {
            if (!visited[to]) {
                dfs(to);
            }
        }
    };

    dfs(start);
    if ((int)order.size() != n) return {false, 0.0, {}};

    order.push_back(start);

    double totalCost = 0.0;
    for (size_t i = 0; i + 1 < order.size(); ++i) {
        int u = order[i];
        int v = order[i + 1];
        if (w[u][v] >= INF / 2) return {false, 0.0, {}};
        totalCost += w[u][v];
    }

    return {true, totalCost, order};
}

TEST(ApproxTSP, TwoVertices) {
    int start = 0;
    vector<vector<double>> w = {
        {INF, 5.0},
        {5.0, INF}
    };
    auto res = approx_tsp(start, w);
    EXPECT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 10.0);
    vector<int> expected = {0, 1, 0};
    EXPECT_EQ(res.path, expected);
}

TEST(ApproxTSP, MissingReturnEdge) {
    int start = 0;
    vector<vector<double>> w = {
        {INF, 1.0, INF},
        {1.0, INF, 2.0},
        {INF, 2.0, INF}
    };
    auto res = approx_tsp(start, w);
    EXPECT_FALSE(res.found);
}

TEST(ApproxTSP, ZeroWeights) {
    int start = 0;
    vector<vector<double>> w = {
        {INF, 0.0, 0.0},
        {0.0, INF, 0.0},
        {0.0, 0.0, INF}
    };
    auto res = approx_tsp(start, w);
    EXPECT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 0.0);
    EXPECT_EQ(res.path.size(), 4);
    EXPECT_EQ(res.path.front(), 0);
    EXPECT_EQ(res.path.back(), 0);
}

TEST(ApproxTSP, DisconnectedGraph) {
    int start = 0;
    vector<vector<double>> w = {
        {INF, 1.0, INF},
        {1.0, INF, INF},
        {INF, INF, INF}
    };
    auto res = approx_tsp(start, w);
    EXPECT_FALSE(res.found);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}