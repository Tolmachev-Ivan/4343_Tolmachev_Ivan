#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <sstream>

using namespace std;

const int ALPH_SIZE = 5;
ostringstream debug;

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

char int_to_char(int idx) {
    static const char chars[] = {'A', 'C', 'G', 'T', 'N'};
    if (idx >= 0 && idx < ALPH_SIZE) return chars[idx];
    return '?';
}

struct Vertex {
    int next[ALPH_SIZE];
    int fail;
    int out;
    vector<int> offsets;
    int depth;

    Vertex() {
        fill(next, next + ALPH_SIZE, -1);
        fail = 0;
        out = -1;
        depth = 0;
    }
};

vector<Vertex> trie(1);

void add_fragment(const string& s, int offset) {
    debug << "  Добавление фрагмента \"" << s << "\" со смещением " << offset << ":\n";
    int v = 0;
    for (size_t pos = 0; pos < s.length(); ++pos) {
        char c = s[pos];
        int idx = char_to_idx(c);
        if (trie[v].next[idx] == -1) {
            trie[v].next[idx] = trie.size();
            trie.emplace_back();
            trie.back().depth = trie[v].depth + 1;
            debug << "    символ '" << c << "': создана вершина " << trie.size() - 1
                  << " (глубина " << trie.back().depth << "), переход из " << v << "\n";
        } else {
            debug << "    символ '" << c << "': переход в существующую вершину " << trie[v].next[idx] << "\n";
        }
        v = trie[v].next[idx];
    }
    trie[v].offsets.push_back(offset);
    debug << "    фрагмент завершён в вершине " << v << ", добавлено смещение " << offset << "\n";
}

void build_automaton() {
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

    debug << "\n\n=========== ПОСТРОЕНИЕ АВТОМАТА ==========\n";
    while (!q.empty()) {
        int v = q.front(); q.pop();
        debug << "Обрабатываю вершину " << v << " (глубина " << trie[v].depth << ")\n";
        if (!trie[trie[v].fail].offsets.empty()) {
            trie[v].out = trie[v].fail;
            debug << "  Конечная ссылка = суффиксная ссылка " << trie[v].fail << " (терминальная)\n";
        } else {
            trie[v].out = trie[trie[v].fail].out;
            debug << "  Конечная ссылка унаследована от " << trie[v].fail << ": " << trie[v].out << "\n";
        }

        for (int c = 0; c < ALPH_SIZE; ++c) {
            int u = trie[v].next[c];
            if (u != -1) {
                trie[u].fail = trie[trie[v].fail].next[c];
                debug << "  Символ '" << int_to_char(c) << "': переход в " << u
                      << ", суффиксная ссылка = " << trie[u].fail << "\n";
                q.push(u);
            } else {
                trie[v].next[c] = trie[trie[v].fail].next[c];
            }
        }
    }
}

void print_automaton() {
    debug << "\n\n========== ПОСТРОЕННЫЙ АВТОМАТ ===========\n";
    for (size_t i = 0; i < trie.size(); ++i) {
        debug << "Вершина " << i;
        if (i == 0) debug << " (корень)";
        debug << ":\n";
        debug << "  Глубина: " << trie[i].depth << "\n";
        debug << "  Переходы:";
        for (int c = 0; c < ALPH_SIZE; ++c) {
            debug << " " << int_to_char(c) << "->" << trie[i].next[c];
        }
        debug << "\n";
        debug << "  Суффиксная ссылка (fail): " << trie[i].fail << "\n";
        debug << "  Конечная ссылка (out): " << (trie[i].out == -1 ? -1 : trie[i].out) << "\n";
        if (!trie[i].offsets.empty()) {
            debug << "  Терминальная вершина, смещения фрагментов:";
            for (int off : trie[i].offsets) debug << " " << off;
            debug << "\n";
        } else {
            debug << "  Не терминальная\n";
        }
        debug << "-----------------------------\n";
    }
}

int main() {
    string T, P;
    char wildcard;
    cin >> T >> P >> wildcard;

    debug << "\n\n============= ВХОДНЫЕ ДАННЫЕ =============\n";
    debug << "Текст: " << T << "\n";
    debug << "Образец: " << P << "\n";
    debug << "Символ джокера: '" << wildcard << "'\n";

    debug << "\n\n===== РАЗБИЕНИЕ ОБРАЗЦА НА ФРАГМЕНТЫ =====\n";
    vector<pair<string, int>> fragments;
    string current;
    int lenP = (int)P.length();

    for (int i = 0; i < lenP; ++i) {
        if (P[i] == wildcard) {
            if (!current.empty()) {
                int offset = i - (int)current.length();
                fragments.emplace_back(current, offset);
                debug << "Фрагмент \"" << current << "\" на позициях [" << offset
                      << ", " << i - 1 << "] (смещение " << offset << ")\n";
                current.clear();
            }
        } else {
            current += P[i];
        }
    }
    if (!current.empty()) {
        int offset = lenP - (int)current.length();
        fragments.emplace_back(current, offset);
        debug << "Фрагмент \"" << current << "\" на позициях [" << offset
              << ", " << lenP - 1 << "] (смещение " << offset << ")\n";
    }

    int frag_count = (int)fragments.size();
    debug << "Всего фрагментов: " << frag_count << "\n";

    if (frag_count == 0) {
        return 0;
    }

    debug << "\n\n============ ПОСТРОЕНИЕ БОРА =============\n";
    for (const auto& f : fragments) {
        add_fragment(f.first, f.second);
    }
    debug << "Бор построен, количество вершин: " << trie.size() << "\n";

    build_automaton();
    print_automaton();

    vector<int> C(T.length() + 2, 0);

    debug << "\n\n============ ПОИСК ПО ТЕКСТУ =============\n";
    int current_state = 0;
    for (int i = 0; i < (int)T.length(); ++i) {
        char c = T[i];
        int idx = char_to_idx(c);
        int prev = current_state;
        current_state = trie[current_state].next[idx];
        debug << "Позиция " << i + 1 << " ('" << c << "'): переход из " << prev << " в " << current_state;

        int temp = current_state;
        bool found_any = false;
        while (temp != 0) {
            if (!trie[temp].offsets.empty()) {
                if (!found_any) {
                    debug << " [обнаружены фрагменты: ";
                    found_any = true;
                }
                for (int off : trie[temp].offsets) {
                    int start = i - trie[temp].depth - off + 2;
                    debug << "фрагмент(смещ. " << off << ") start=" << start;
                    if (start >= 1 && start <= (int)T.length() - (int)P.length() + 1) {
                        debug << "(валиден, C[" << start << "]++) ";
                        C[start]++;
                    } else {
                        debug << "(не валиден) ";
                    }
                }
            }
            temp = trie[temp].out;
            if (temp == -1) break;
        }
        if (found_any) debug << "\b]";
        debug << "\n";
    }

    debug << "\n\n=============== РЕЗУЛЬТАТ ================\n";
    bool first = true;
    for (int i = 1; i <= (int)T.length(); ++i) {
        if (C[i] == frag_count) {
            if (first) debug << "Позиции вхождений: ";
            debug << i << " ";
            cout << i << '\n';
            first = false;
        }
    }
    if (first) debug << "Вхождений не найдено.\n";
    debug << "\n";

    debug << "\n\n============ ИНДИВИДУАЛИЗАЦИЯ ============\n";
    debug << "Количество вершин в автомате: " << trie.size() << "\n";

    cout << debug.str();

    return 0;
}