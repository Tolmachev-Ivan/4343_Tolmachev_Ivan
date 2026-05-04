#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<int> compute_prefix_function(const string& pattern) {
    int m = pattern.size();
    vector<int> prefix(m, 0);
    cout << "\n\n=========== ПОСТРОЕНИЕ ПРЕФИКС ФУНКЦИИ ===========" << endl;
    cout << "Образец: \"" << pattern << "\", длина m = " << m << endl;
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
    
    cout << "\nМассив префикс-функции: [";
    for (int i = 0; i < m; ++i) {
        if (i > 0) cout << ", ";
        cout << prefix[i];
    }
    cout << "]" << endl;
    return prefix;
}

int main() {
    string P, T;
    cout << "Образец P: ";
    getline(cin, P);
    cout << "Текст T: ";
    getline(cin, T);
    
    cout << "\n\n================= ВВОДНЫЕ ДАННЫЕ =================" << endl;
    cout << "Образец P: \"" << P << "\" (длина " << P.size() << ")" << endl;
    cout << "Текст T:   \"" << T << "\" (длина " << T.size() << ")" << endl;
    
    if (P.empty()) {
        cout << "Образец пустой -> вхождений нет." << endl;
        cout << -1;
        return 0;
    }

    vector<int> prefix = compute_prefix_function(P);
    vector<int> positions;
    int n = T.size(), m = P.size();
    int j = 0;

    cout << "\n\n================ ПОИСК ВХОЖДЕНИЙ =================" << endl;
    cout << "Движемся по тексту, i — индекс в T, j — индекс в P." << endl;
    cout << "Начальное j = 0" << endl;
    
    for (int i = 0; i < n; ++i) {
        cout << "\n--- Символ текста i=" << i << " ('" << T[i] << "') ---" << endl;
        cout << "Текущее j = " << j;
        if (j < m) cout << " (символ образца P[" << j << "]='" << P[j] << "')";
        cout << endl;
        
        while (j > 0 && T[i] != P[j]) {
            cout << "  Несовпадение: T[" << i << "]='" << T[i]
                 << "' != P[" << j << "]='" << P[j] << "'" << endl;
            int old_j = j;
            j = prefix[j - 1];
            cout << "  Откат по префикс функции: j = prefix[" << old_j - 1 << "] = " << j;
            if (j < m) cout << " (теперь P[" << j << "]='" << P[j] << "')";
            cout << endl;
        }
        
        if (T[i] == P[j]) {
            cout << "  Совпадение: T[" << i << "]='" << T[i]
                 << "' == P[" << j << "]='" << P[j] << "' -> j увеличивается на 1" << endl;
            ++j;
        } else {
            cout << "  Нет совпадения, j остаётся " << j << endl;
        }
        
        if (j == m) {
            int start_pos = i - m + 1;
            cout << "  >>> Найдено вхождение: начало в позиции " << start_pos << " <<<" << endl;
            positions.push_back(start_pos);
            j = prefix[j - 1];
            cout << "  Для поиска перекрывающихся вхождений j = prefix[" << m-1 << "] = " << j << endl;
        }
    }
    
    cout << "\n\n=================== РЕЗУЛЬТАТ ====================" << endl;
    if (positions.empty()) {
        cout << "Вхождений не найдено." << endl;
        cout << -1;
    } else {
        cout << "Найденные начальные индексы: ";
        for (size_t k = 0; k < positions.size(); ++k) {
            if (k > 0) cout << ',';
            cout << positions[k];
        }
        cout << endl;
    }
    
    return 0;
}