#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int base_replace, base_insert, base_delete;
    cout << "Стоимости операций (R, I, D): ";
    cin >> base_replace >> base_insert >> base_delete;

    pair<char, int> special_replace;
    char special_char;
    int cost;
    cout << "Особый символ и его замена: ";
    cin >> special_char >> cost;
    special_replace = {special_char, cost};

    pair<char, int> special_insert;
    cout << "Особый символ и его вставка: ";
    cin >> special_char >> cost;
    special_insert = {special_char, cost};

    string A, B;
    cout << "Строка A: ";
    cin >> A;
    cout << "Строка B: ";
    cin >> B;

    cout << "\n============= ВВЕДЁННЫЕ ДАННЫЕ ==============\n";
    cout << "Базовые стоимости: замена = " << base_replace
         << ", вставка = " << base_insert
         << ", удаление = " << base_delete << "\n\n";

    cout << "Особая цена замены: "
         << "символ '" << special_replace.first << "' -> "
         << special_replace.second << "\n";

    cout << "Особая цена вставки: "
         << "символ '" << special_insert.first << "' -> "
         << special_insert.second << "\n\n";

    cout << "Строка A = \"" << A << "\"\n";
    cout << "Строка B = \"" << B << "\"\n\n";

    int n = A.size();
    int m = B.size();

    vector<vector<int>> dp(n + 1, vector<int>(m + 1));

    for (int i = 0; i <= n; ++i) {
        dp[i][0] = i * base_delete;
    }
    for (int j = 1; j <= m; ++j) {
        char c = B[j - 1];
        int ins_cost = (special_insert.first == c) ? special_insert.second : base_insert;
        dp[0][j] = dp[0][j - 1] + ins_cost;
    }

    cout << "========== ЗАПОЛНЕНИЕ ТАБЛИЦЫ DP ==========\n";
    cout << "Пустая строка и столбец уже инициализированы.\n\n";

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            char a_char = A[i - 1];
            char b_char = B[j - 1];

            cout << "--- Ячейка i=" << i << " ('" << a_char << "'), j=" << j << " ('" << b_char << "') ---\n";

            if (a_char == b_char) {
                int val = dp[i - 1][j - 1];
                dp[i][j] = val;
                cout << "  Символы совпадают: '" << a_char << "' == '" << b_char << "'\n";
                cout << "  dp[" << i << "][" << j << "] = dp[" << i-1 << "][" << j-1 << "] = " << val << "\n";
            } else {
                int rep_cost = (special_replace.first == a_char) ? special_replace.second : base_replace;
                int ins_cost = (special_insert.first == b_char) ? special_insert.second : base_insert;
                int del_cost = base_delete;

                int replace = dp[i - 1][j - 1] + rep_cost;
                int insert  = dp[i][j - 1] + ins_cost;
                int del     = dp[i - 1][j] + del_cost;

                cout << "  Символы разные. Варианты:\n";
                cout << "    1) Замена '" << a_char << "' -> '" << b_char << "': "
                     << "rep_cost = " << rep_cost
                     << " (особый: " << (special_replace.first == a_char ? "да" : "нет") << ")"
                     << ", replace = dp[" << i-1 << "][" << j-1 << "] + " << rep_cost
                     << " = " << dp[i-1][j-1] << " + " << rep_cost << " = " << replace << "\n";
                cout << "    2) Вставка '" << b_char << "': "
                     << "ins_cost = " << ins_cost
                     << " (особый: " << (special_insert.first == b_char ? "да" : "нет") << ")"
                     << ", insert = dp[" << i << "][" << j-1 << "] + " << ins_cost
                     << " = " << dp[i][j-1] << " + " << ins_cost << " = " << insert << "\n";
                cout << "    3) Удаление '" << a_char << "': "
                     << "del_cost = " << del_cost
                     << ", del = dp[" << i-1 << "][" << j << "] + " << del_cost
                     << " = " << dp[i-1][j] << " + " << del_cost << " = " << del << "\n";

                int minimum = min({replace, insert, del});
                dp[i][j] = minimum;
                cout << "  -> минимум = " << minimum << " (";
                if (minimum == replace) cout << "замена";
                else if (minimum == insert) cout << "вставка";
                else cout << "удаление";
                cout << ")\n";
            }
            cout << "  Текущее dp[" << i << "][" << j << "] = " << dp[i][j] << "\n\n";
        }
    }

    cout << "================ ТАБЛИЦА DP =================\n";
    cout << "       ";
    for (int j = 0; j <= m; ++j) {
        if (j == 0)
            cout << " ε";
        else
            cout << "    " << B[j - 1];
    }
    cout << "\n";

    for (int i = 0; i <= n; ++i) {
        if (i == 0)
            cout << "  ε ";
        else
            cout << "  " << A[i - 1] << " ";
        for (int j = 0; j <= m; ++j) {
            cout.width(5);
            cout << dp[i][j];
        }
        cout << "\n";
    }
    cout << "\nМинимальная стоимость преобразования: " << dp[n][m] << "\n\n";

    cout << "========== ВОССТАНОВЛЕНИЕ ОПЕРАЦИЙ ==========\n";
    string ops;
    int i = n, j = m;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && A[i - 1] == B[j - 1]) {
            cout << "  " << A[i - 1] << " == " << B[j - 1] << " -> M\n";
            ops.push_back('M');
            --i; --j;
        } else {
            char a_char = (i > 0) ? A[i - 1] : '\0';
            char b_char = (j > 0) ? B[j - 1] : '\0';

            int rep_cost = (i > 0 && special_replace.first == a_char) ? special_replace.second : base_replace;
            int ins_cost = (j > 0 && special_insert.first == b_char) ? special_insert.second : base_insert;
            int del_cost = base_delete;

            if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + rep_cost) {
                cout << "  Замена " << a_char << " -> " << b_char
                          << " (цена " << rep_cost << ") -> R\n";
                ops.push_back('R');
                --i; --j;
            } else if (j > 0 && dp[i][j] == dp[i][j - 1] + ins_cost) {
                cout << "  Вставка " << b_char
                          << " (цена " << ins_cost << ") -> I\n";
                ops.push_back('I');
                --j;
            } else if (i > 0 && dp[i][j] == dp[i - 1][j] + del_cost) {
                cout << "  Удаление " << a_char
                          << " (цена " << del_cost << ") -> D\n";
                ops.push_back('D');
                --i;
            } else {
                if (i > 0 && j > 0) {
                    cout << "  (fallback) Замена " << a_char << " -> " << b_char << " -> R\n";
                    ops.push_back('R'); --i; --j;
                } else if (j > 0) {
                    cout << "  (fallback) Вставка " << b_char << " -> I\n";
                    ops.push_back('I'); --j;
                } else {
                    cout << "  (fallback) Удаление " << a_char << " -> D\n";
                    ops.push_back('D'); --i;
                }
            }
        }
    }

    reverse(ops.begin(), ops.end());

    cout << "\nВосстановленная последовательность: " << ops << "\n\n";

    return 0;
}