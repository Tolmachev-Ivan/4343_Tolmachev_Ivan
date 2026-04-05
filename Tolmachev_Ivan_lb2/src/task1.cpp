#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <limits>
#include <fstream>
#include <sstream>

using namespace std;

const double INF = 1e100;
const double EPS = 1e-12;

istream* chooseInputStream(int argc, char* argv[], ifstream& fin) {
    if (argc > 1) {
        fin.open(argv[1]);
        if (fin) return &fin;
    } else {
        fin.open("input.txt");
        if (fin) return &fin;
    }
    return &cin;
}

void printMatrix(const vector<vector<double>>& w) {
    int n = (int)w.size();
    cout << "Матрица весов:\n";
    cout << fixed << setprecision(2);
    for (int i = 0; i < n; ++i) {
        cout << "  ";
        for (int j = 0; j < n; ++j) {
            if (j) cout << ' ';
            if (w[i][j] >= INF / 2) cout << setw(8) << "INF";
            else cout << setw(8) << w[i][j];
        }
        cout << '\n';
    }
    cout << '\n';
}

void printVectorInt(const vector<int>& vec, const string& title) {
    cout << title;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i) cout << ' ';
        cout << vec[i];
    }
    cout << '\n';
}

void printStepMinEdges(const vector<double>& min_edge, const vector<bool>& used) {
    cout << "  Текущие минимальные рёбра до непосещённых вершин:\n";
    cout << fixed << setprecision(2);
    cout << "  ";
    for (size_t i = 0; i < min_edge.size(); ++i) {
        cout << "v" << i << "=";
        if (used[i]) cout << "[visited]";
        else if (min_edge[i] >= INF / 2) cout << "INF";
        else cout << min_edge[i];
        if (i + 1 < min_edge.size()) cout << "   ";
    }
    cout << '\n';
}

void printTreeEdges(const vector<vector<pair<int, double>>>& tree) {
    cout << "Рёбра минимального остовного дерева:\n";
    cout << fixed << setprecision(2);
    for (size_t v = 0; v < tree.size(); ++v) {
        for (const auto& [to, weight] : tree[v]) {
            if ((int)v < to) {
                cout << "  " << v << " -- " << to << "  (" << weight << ")\n";
            }
        }
    }
    cout << '\n';
}

int main(int argc, char* argv[]) {
    ifstream fin;
    istream* in = chooseInputStream(argc, argv, fin);

    int start;
    if (!(*in >> start)) {
        cout << "no path\n";
        return 0;
    }

    vector<double> vals;
    double x;
    while (*in >> x) {
        vals.push_back(x);
    }

    long long n_sq = llround(sqrt((long double)vals.size()));
    if (n_sq * n_sq != (long long)vals.size()) {
        cout << "no path\n";
        return 0;
    }
    int n = (int)n_sq;

    if (start < 0 || start >= n) {
        cout << "no path\n";
        return 0;
    }

    vector<vector<double>> w(n, vector<double>(n, INF));
    for (int i = 0, idx = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j, ++idx) {
            if (i == j) continue;
            double v = vals[idx];
            if (v >= 0) {
                w[i][j] = v;
            }
        }
    }

    cout << "========================================\n";
    cout << "Стартовая вершина: " << start << "\n\n";
    printMatrix(w);

    vector<double> min_edge(n, INF);
    vector<int> parent(n, -1);
    vector<bool> used(n, false);

    min_edge[start] = 0.0;

    cout << "Построение МОД алгоритмом Прима:\n";
    for (int it = 0; it < n; ++it) {
        int v = -1;
        for (int i = 0; i < n; ++i) {
            if (!used[i] && (v == -1 || min_edge[i] < min_edge[v])) {
                v = i;
            }
        }

        if (v == -1 || min_edge[v] >= INF / 2) {
            cout << "Граф несвязный или недостижимые вершины.\n";
            cout << "no path\n";
            return 0;
        }

        cout << "Шаг " << (it + 1) << ":\n";
        cout << "  Выбираем вершину " << v;
        if (parent[v] != -1) {
            cout << " через ребро " << parent[v] << " -- " << v
                 << " весом " << fixed << setprecision(2) << min_edge[v];
        } else {
            cout << " (стартовая вершина)";
        }
        cout << '\n';

        used[v] = true;

        for (int to = 0; to < n; ++to) {
            if (!used[to] && w[v][to] < min_edge[to]) {
                min_edge[to] = w[v][to];
                parent[to] = v;
            }
        }

        printStepMinEdges(min_edge, used);
        cout << '\n';
    }

    vector<vector<pair<int, double>>> tree(n);
    double mst_cost = 0.0;

    for (int v = 0; v < n; ++v) {
        if (v == start) continue;
        if (parent[v] == -1) {
            cout << "no path\n";
            return 0;
        }
        double weight = w[v][parent[v]];
        tree[v].emplace_back(parent[v], weight);
        tree[parent[v]].emplace_back(v, weight);
        mst_cost += weight;
    }

    for (int v = 0; v < n; ++v) {
        sort(tree[v].begin(), tree[v].end(),
             [](const pair<int,double>& a, const pair<int,double>& b) {
                 if (fabs(a.second - b.second) > EPS) return a.second < b.second;
                 return a.first < b.first;
             });
    }

    cout << "Итоговое МОД:\n";
    printTreeEdges(tree);
    cout << "Стоимость МОД: " << fixed << setprecision(2) << mst_cost << "\n\n";

    vector<int> order;
    vector<bool> visited(n, false);

    cout << "Двойной обход МОД (DFS от стартовой вершины):\n";
    function<void(int,int)> dfs_traverse = [&](int v, int depth) {
        visited[v] = true;
        order.push_back(v);

        cout << string(depth * 2, ' ') << "Посещаем вершину " << v << '\n';

        for (const auto& [to, weight] : tree[v]) {
            if (!visited[to]) {
                cout << string(depth * 2, ' ') << "  Переход по ребру "
                     << v << " -- " << to << " (" << fixed << setprecision(2)
                     << weight << ")\n";
                dfs_traverse(to, depth + 1);
                cout << string(depth * 2, ' ') << "  Возврат к " << v << '\n';
            }
        }
    };

    dfs_traverse(start, 0);

    if ((int)order.size() != n) {
        cout << "no path\n";
        return 0;
    }

    cout << '\n';
    printVectorInt(order, "Порядок первых посещений: ");

    order.push_back(start);
    printVectorInt(order, "Маршрут с возвратом в стартовую вершину: ");

    double total_cost = 0.0;
    cout << "\nПодсчёт стоимости маршрута:\n";
    for (size_t i = 0; i + 1 < order.size(); ++i) {
        int u = order[i];
        int v = order[i + 1];
        if (w[u][v] >= INF / 2) {
            cout << "  Нет ребра " << u << " -> " << v << "\n";
            cout << "no path\n";
            return 0;
        }
        cout << "  " << u << " -> " << v << " = " << fixed << setprecision(2) << w[u][v] << '\n';
        total_cost += w[u][v];
    }

    cout << "\nИтог:\n";
    cout << "  Стоимость маршрута: " << fixed << setprecision(2) << total_cost << "\n";
    cout << "  Путь: ";
    for (size_t i = 0; i < order.size(); ++i) {
        if (i) cout << ' ';
        cout << order[i];
    }
    cout << '\n';

    return 0;
}