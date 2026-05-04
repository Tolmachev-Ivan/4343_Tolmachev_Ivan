#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<int> compute_prefix_function(const string& pattern) {
    int m = pattern.size();
    vector<int> prefix(m, 0);
    cout << "\n\n=========== ПОСТРОЕНИЕ ПРЕФИКС ФУНКЦИИ ===========" << endl;
    cout << "Образец B: \"" << pattern << "\", длина m = " << m << endl;
    cout << "i=0: prefix[0] = 0 (всегда)" << endl;
    
    for (int i = 1; i < m; ++i) {
        int j = prefix[i - 1];
        cout << "\n--- Шаг i=" << i << " (символ '" << pattern[i] << "') ---" << endl;
        cout << "Начальное j = prefix[" << i-1 << "] = " << j << endl;
        
        while (j > 0 && pattern[i] != pattern[j]) {
            cout << "  Несовпадение: pattern[" << i << "]='" << pattern[i]
                 << "' != pattern[" << j << "]='" << pattern[j] << "'" << endl;
            int old_j = j;
            j = prefix[j - 1];
            cout << "  Откат: j = prefix[" << old_j - 1 << "] = " << j << endl;
        }
        
        if (pattern[i] == pattern[j]) {
            cout << "  Совпадение: pattern[" << i << "]='" << pattern[i]
                 << "' == pattern[" << j << "]='" << pattern[j] << "' -> увеличиваем j" << endl;
            ++j;
        } else {
            cout << "  Нет совпадения и j=0, оставляем j=" << j << endl;
        }
        
        prefix[i] = j;
        cout << "Итог: prefix[" << i << "] = " << prefix[i] << endl;
    }
    
    cout << "\nМассив префикс функции: [";
    for (int i = 0; i < m; ++i) {
        if (i > 0) cout << ", ";
        cout << prefix[i];
    }
    cout << "]" << endl;
    return prefix;
}

int main() {
    string A, B;
    cout << "Первая строка A: ";
    getline(cin, A);
    cout << "Вторая строка B: ";
    getline(cin, B);

    int n = A.size(), m = B.size();
    cout << "\n\n================= ВВОДНЫЕ ДАННЫЕ =================" << endl;
    cout << "A: \"" << A << "\" (длина " << n << ")" << endl;
    cout << "B: \"" << B << "\" (длина " << m << ")" << endl;

    if (n != m) {
        cout << "Длины строк не совпадают -> A не может быть циклическим сдвигом B." << endl;
        cout << "Результат: -1" << endl;
        return 0;
    }
    if (n == 0) {
        cout << "Обе строки пусты -> циклический сдвиг тривиален, индекс начала B в A = 0." << endl;
        cout << "Результат: 0" << endl;
        return 0;
    }

    vector<int> prefix = compute_prefix_function(B);

    int j = 0;
    int total = 2 * n;
    cout << "\n=== ПОИСК ОБРАЗЦА B В ВИРТУАЛЬНОЙ СТРОКЕ A+A ===" << endl;
    cout << "Виртуальная строка (A+A) длиной " << total << " символов." << endl;
    cout << "Индексы виртуальной строки i от 0 до " << total-1 << "." << endl;
    cout << "Символ cur = A[i % n] (циклический доступ к A)." << endl;
    cout << "Начальное j = 0" << endl;

    for (int i = 0; i < total; ++i) {
        char cur = A[i % n];
        cout << "\n--- Шаг i=" << i << " (A[" << i % n << "]='" << cur << "') ---" << endl;
        cout << "Текущее j = " << j;
        if (j < m) cout << " (символ образца B[" << j << "]='" << B[j] << "')";
        cout << endl;

        while (j > 0 && cur != B[j]) {
            cout << "  Несовпадение: cur='" << cur
                 << "' != B[" << j << "]='" << B[j] << "'" << endl;
            int old_j = j;
            j = prefix[j - 1];
            cout << "  Откат по префикс функции: j = prefix[" << old_j - 1 << "] = " << j;
            if (j < m) cout << " (теперь B[" << j << "]='" << B[j] << "')";
            cout << endl;
        }

        if (cur == B[j]) {
            cout << "  Совпадение: cur='" << cur
                 << "' == B[" << j << "]='" << B[j] << "' -> j увеличивается на 1" << endl;
            ++j;
        } else {
            cout << "  Нет совпадения, j остаётся " << j << endl;
        }

        if (j == m) {
            int start_pos = i - m + 1;
            cout << "  >>> НАЙДЕНО ПОЛНОЕ ВХОЖДЕНИЕ: начало вхождения в A+A " << start_pos << endl;

            if (start_pos < n) {
                cout << "  Индекс начала B в A (start_pos < n): " << start_pos << endl;
                cout << "\n\n=================== РЕЗУЛЬТАТ ====================" << endl;
                cout << start_pos << endl;
                return 0;
            } else {
                cout << "  Вхождение начинается за пределами первой копии A (start_pos >= n), пропускаем." << endl;
            }

            j = prefix[j - 1];
            cout << "  Для поиска перекрывающихся вхождений j = prefix[" << m-1 << "] = " << j << endl;
        }
    }

    cout << "\n\n=================== РЕЗУЛЬТАТ ====================" << endl;
    cout << "Вхождение не найдено, A не является циклическим сдвигом B." << endl;
    cout << -1 << endl;
    return 0;
}