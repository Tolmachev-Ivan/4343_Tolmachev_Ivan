#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

const double INF = 1e100;
const double EPS = 1e-9;

double best_cost = INF;
vector<int> best_path;

string indent(int depth) {
    return string(depth * 2, ' ');
}

void printMatrix(const vector<vector<double>>& mat,
                        const vector<int>& rows,
                        const vector<int>& cols,
                        int depth,
                        const string& title) {
    cout << indent(depth) << title << '\n';
    cout << indent(depth) << "      ";
    for (int j = 0; j < (int)cols.size(); ++j) {
        cout << setw(8) << cols[j];
    }
    cout << '\n';

    for (int i = 0; i < (int)mat.size(); ++i) {
        cout << indent(depth) << setw(4) << rows[i] << "  ";
        for (int j = 0; j < (int)mat[i].size(); ++j) {
            if (mat[i][j] >= INF / 2) {
                cout << setw(8) << "INF";
            } else {
                ostringstream oss;
                oss << fixed << setprecision(1) << mat[i][j];
                cout << setw(8) << oss.str();
            }
        }
        cout << '\n';
    }
    cout << '\n';
}

void printInitialMatrix(const vector<vector<double>>& w) {
    int n = (int)w.size();
    cout << "Исходная матрица весов:\n";
    cout << "      ";
    for (int j = 0; j < n; ++j) {
        cout << setw(8) << j;
    }
    cout << '\n';

    for (int i = 0; i < n; ++i) {
        cout << setw(4) << i << "  ";
        for (int j = 0; j < n; ++j) {
            if (w[i][j] >= INF / 2) {
                cout << setw(8) << "INF";
            } else {
                ostringstream oss;
                oss << fixed << setprecision(1) << w[i][j];
                cout << setw(8) << oss.str();
            }
        }
        cout << '\n';
    }
    cout << '\n';
}

void printCurrentArcs(const vector<int>& next_arc, int n, int depth) {
    cout << indent(depth) << "Текущие включённые дуги: ";
    bool any = false;
    for (int i = 0; i < n; ++i) {
        if (next_arc[i] != -1) {
            if (any) cout << ", ";
            cout << i << "->" << next_arc[i];
            any = true;
        }
    }
    if (!any) cout << "(нет)";
    cout << '\n';
}

double reduceMatrix(vector<vector<double>>& mat,
                    const vector<int>& rows,
                    const vector<int>& cols,
                    int depth = 0,
                    const string& stage = "") {
    int n = (int)rows.size();
    int m = (int)cols.size();
    double total = 0.0;

    if (!stage.empty()) {
        cout << indent(depth) << stage << '\n';
    }

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

    if (!stage.empty()) {
        cout << indent(depth) << "Прирост нижней оценки после приведения: "
             << fixed << setprecision(1) << total << '\n';
        printMatrix(mat, rows, cols, depth, "Приведённая матрица:");
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
                    int n,
                    int depth = 0) {
    int k = (int)rows.size();
    int l = (int)cols.size();

    cout << indent(depth) << "----------------------------------------\n";
    cout << indent(depth) << "Узел дерева поиска\n";
    cout << indent(depth) << "Текущая нижняя оценка: "
         << fixed << setprecision(1) << lower_bound << '\n';
    printCurrentArcs(next_arc, n, depth);

    if (lower_bound >= best_cost - EPS) {
        cout << indent(depth) << "Отсечение: нижняя оценка не лучше текущего лучшего решения ("
             << fixed << setprecision(1) << best_cost << ").\n\n";
        return;
    }

    if (k == 1 && l == 1) {
        int u = rows[0];
        int v = cols[0];

        cout << indent(depth) << "Достигнут лист дерева. Осталась единственная дуга: "
             << u << "->" << v << '\n';

        if (next_arc[u] != -1 || prev_arc[v] != -1) {
            cout << indent(depth) << "Лист отвергнут: конфликт по входящей/исходящей дуге.\n\n";
            return;
        }

        next_arc[u] = v;
        prev_arc[v] = u;

        vector<int> path;
        int cur = 0;
        do {
            path.push_back(cur);
            cur = next_arc[cur];
        } while (cur != 0);

        double cost = lower_bound;
        cout << indent(depth) << "Получен полный цикл: ";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i) cout << ' ';
            cout << path[i];
        }
        cout << " " << path[0] << '\n';
        cout << indent(depth) << "Стоимость цикла: " << fixed << setprecision(1) << cost << '\n';

        if (cost < best_cost - EPS) {
            best_cost = cost;
            best_path = path;
            cout << indent(depth) << "Найдено новое лучшее решение.\n";
        } else {
            cout << indent(depth) << "Решение не лучше текущего лучшего.\n";
        }

        next_arc[u] = -1;
        prev_arc[v] = -1;
        cout << '\n';
        return;
    }

    int best_i = -1, best_j = -1;
    double best_delta = -1.0;
    bool force_left = false;

    cout << indent(depth) << "Поиск нулевого элемента для ветвления...\n";

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

                cout << indent(depth) << "  Ноль (" << rows[i] << "," << cols[j] << ")"
                     << ", штраф = ";
                if (delta >= INF / 2) cout << "INF";
                else cout << fixed << setprecision(1) << delta;
                cout << '\n';

                if (delta >= INF / 2) {
                    force_left = true;
                    best_i = i;
                    best_j = j;
                    best_delta = INF;
                    break;
                }

                if (delta > best_delta + EPS) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }
        if (force_left) break;
    }

    if (best_i == -1) {
        cout << indent(depth) << "Нулевых элементов нет, ветвление невозможно.\n\n";
        return;
    }

    int u = rows[best_i];
    int v = cols[best_j];

    cout << indent(depth) << "Выбран ноль для ветвления: (" << u << "," << v << ")";
    if (best_delta < INF / 2) {
        cout << ", штраф = " << fixed << setprecision(1) << best_delta;
    }
    cout << '\n';
    cout << indent(depth) << "Соответствующая дуга: " << u << "->" << v << '\n';

    if (!force_left) {
        cout << indent(depth) << "Правая ветвь: запрещаем дугу " << u << "->" << v << '\n';
        vector<vector<double>> mat_right = mat;
        mat_right[best_i][best_j] = INF;
        double add = reduceMatrix(mat_right, rows, cols, depth + 1, "Редукция после запрета дуги:");
        double new_bound = lower_bound + add;

        cout << indent(depth) << "Нижняя оценка правой ветви: "
             << fixed << setprecision(1) << new_bound << '\n';

        if (new_bound < best_cost - EPS) {
            branchAndBound(mat_right, rows, cols, new_bound,
                           next_arc, prev_arc, included_count, n, depth + 1);
        } else {
            cout << indent(depth) << "Правая ветвь отсечена.\n\n";
        }
    } else {
        cout << indent(depth) << "Правая ветвь не рассматривается: ноль с бесконечным штрафом.\n";
    }

    if (next_arc[u] != -1 || prev_arc[v] != -1) {
        cout << indent(depth) << "Левая ветвь невозможна: конфликт по входящей/исходящей дуге.\n\n";
        return;
    }

    if (included_count < n - 1) {
        int x = v;
        while (x != -1) {
            if (x == u) {
                cout << indent(depth) << "Левая ветвь отсечена: образуется подцикл.\n\n";
                return;
            }
            x = next_arc[x];
        }
    }

    cout << indent(depth) << "Левая ветвь: включаем дугу " << u << "->" << v << '\n';

    vector<int> new_rows, new_cols;
    for (int i = 0; i < k; ++i) if (i != best_i) new_rows.push_back(rows[i]);
    for (int j = 0; j < l; ++j) if (j != best_j) new_cols.push_back(cols[j]);

    int new_k = (int)new_rows.size();
    int new_l = (int)new_cols.size();
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

    double add = reduceMatrix(mat_left, new_rows, new_cols, depth + 1, "Редукция после включения дуги:");
    double new_bound = lower_bound + add;

    cout << indent(depth) << "Нижняя оценка левой ветви: "
         << fixed << setprecision(1) << new_bound << '\n';

    if (new_bound < best_cost - EPS) {
        branchAndBound(mat_left, new_rows, new_cols, new_bound,
                       next_arc, prev_arc, included_count + 1, n, depth + 1);
    } else {
        cout << indent(depth) << "Левая ветвь отсечена.\n\n";
    }

    next_arc[u] = -1;
    prev_arc[v] = -1;
}

istream* chooseInputStream(int argc, char* argv[], ifstream& fin) {
    if (argc > 1) {
        fin.open(argv[1]);
        if (fin) return &fin;
    }
    return &cin;
}

int main(int argc, char* argv[]) {
    ifstream fin;
    istream* in = chooseInputStream(argc, argv, fin);

    int n;
    if (!(*in >> n)) return 0;

    vector<vector<double>> w(n, vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            *in >> w[i][j];
            if (i == j || w[i][j] < 0) w[i][j] = INF;
        }
    }

    cout << "========================================\n";
    cout << "Размер матрицы: " << n << "\n\n";
    printInitialMatrix(w);

    vector<int> rows(n), cols(n);
    for (int i = 0; i < n; ++i) rows[i] = cols[i] = i;

    vector<vector<double>> mat = w;
    cout << "Начальная редукция матрицы:\n";
    double lower_bound = reduceMatrix(mat, rows, cols, 0, "После приведения:");
    cout << "Начальная нижняя оценка: " << fixed << setprecision(1) << lower_bound << "\n\n";

    vector<int> next_arc(n, -1), prev_arc(n, -1);

    best_cost = INF;
    best_path.clear();

    branchAndBound(mat, rows, cols, lower_bound, next_arc, prev_arc, 0, n, 0);

    cout << "========================================\n";
    if (best_path.empty()) {
        cout << "No solution" << endl;
    } else {
        cout << "Лучший маршрут: ";
        for (size_t i = 0; i < best_path.size(); ++i) {
            if (i > 0) cout << ' ';
            cout << best_path[i];
        }
        cout << ' ' << best_path[0] << '\n';
        cout << "Стоимость: " << fixed << setprecision(1) << best_cost << endl;
    }

    return 0;
}