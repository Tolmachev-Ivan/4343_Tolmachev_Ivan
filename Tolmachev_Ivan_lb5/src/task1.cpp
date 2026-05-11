#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <set>
#include <sstream>

using namespace std;

const int ALPH_SIZE = 5;
ostringstream debug;

char int_to_char(int idx) {
    static const char chars[] = {'A', 'C', 'G', 'T', 'N'};
    if (idx >= 0 && idx < ALPH_SIZE) return chars[idx];
    return '?';
}

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

struct Vertex {
    int next[ALPH_SIZE];
    int fail;
    int out;
    vector<int> patterns;
    int pattern_len;
    
    Vertex() {
        fill(next, next + ALPH_SIZE, -1);
        fail = 0;
        out = -1;
        patterns.clear();
        pattern_len = 0;
    }
};

vector<Vertex> trie(1);

void add_pattern(const string& s, int pat_idx) {
    debug << "Добавление образца " << pat_idx << ": \"" << s << "\"\n";
    int v = 0;
    for (size_t pos = 0; pos < s.length(); ++pos) {
        char c = s[pos];
        int idx = char_to_idx(c);
        if (trie[v].next[idx] == -1) {
            trie[v].next[idx] = trie.size();
            trie.emplace_back();
            debug << "  Символ '" << c << "': создана новая вершина " << trie.size() - 1
                  << ", переход из " << v << " -> " << trie.size() - 1 << "\n";
        } else {
            debug << "  Символ '" << c << "': переход по существующему пути " << v
                  << " -> " << trie[v].next[idx] << "\n";
        }
        v = trie[v].next[idx];
    }
    trie[v].patterns.push_back(pat_idx);
    trie[v].pattern_len = s.length();
    debug << "  Образец " << pat_idx << " добавлен, терминальная вершина " << v
          << " (длина " << s.length() << ")\n";
}

void print_automaton() {
    debug << "\n\n=========== ПОСТРОЕННЫЙ АВТОМАТ ==========\n";
    for (size_t i = 0; i < trie.size(); ++i) {
        debug << "Вершина " << i;
        if (i == 0) debug << " (корень)";
        debug << ":\n";
        debug << "  Переходы:";
        for (int c = 0; c < ALPH_SIZE; ++c) {
            debug << " " << int_to_char(c) << "->" << trie[i].next[c];
        }
        debug << "\n";
        debug << "  Суффиксная ссылка (fail): " << trie[i].fail << "\n";
        debug << "  Конечная ссылка (out): " << (trie[i].out == -1 ? -1 : trie[i].out) << "\n";
        if (!trie[i].patterns.empty()) {
            debug << "  Терминальная вершина, образцы: ";
            for (int p : trie[i].patterns) debug << p << " ";
            debug << "(длина " << trie[i].pattern_len << ")\n";
        } else {
            debug << "  Не терминальная\n";
        }
        debug << "-----------------------------\n";
    }
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
        debug << "Обрабатываю вершину " << v << "\n";
        if (!trie[trie[v].fail].patterns.empty()) {
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
                debug << "  Символ '" << int_to_char(c) << "': переход в " << u << ", fail = " << trie[u].fail << "\n";
                q.push(u);
            } else {
                trie[v].next[c] = trie[trie[v].fail].next[c];
            }
        }
    }
}

int main() {
    string T;
    int n;
    cin >> T >> n;
    
    debug << "\n\n============= ВХОДНЫЕ ДАННЫЕ =============\n";
    debug << "Текст: " << T << "\n";
    debug << "Количество образцов: " << n << "\n";
    
    debug << "\n\n============= ПОСТРОЕНИЕ БОРА ============\n";
    vector<int> pattern_len(n + 1);
    for (int i = 1; i <= n; ++i) {
        string p;
        cin >> p;
        pattern_len[i] = p.length();
        add_pattern(p, i);
    }
    
    debug << "Бор построен, количество вершин: " << trie.size() << "\n";
    
    build_automaton();
    print_automaton();
    
    int text_len = T.length();
    vector<vector<int>> occ_by_start(text_len + 2);
    
        debug << "\n\n============= ПОИСК В ТЕКСТЕ =============\n";
    int current = 0;
    for (int i = 0; i < text_len; ++i) {
        char c = T[i];
        int idx = char_to_idx(c);
        int prev = current;
        current = trie[current].next[idx];
        debug << "Позиция " << i+1 << " ('" << c << "'): переход из " << prev << " в " << current;
        
        int temp = current;
        bool found_any = false;
        while (temp != 0) {
            if (!trie[temp].patterns.empty()) {
                if (!found_any) {
                    debug << " [найдены образцы: ";
                    found_any = true;
                }
                int start = i - trie[temp].pattern_len + 2;
                for (int p_idx : trie[temp].patterns) {
                    debug << "образец " << p_idx << " на позиции " << start << "; ";
                    occ_by_start[start].push_back(p_idx);
                }
            }
            temp = trie[temp].out;
            if (temp == -1) break;
        }
        if (found_any) debug << "\b\b]";
        debug << "\n";
    }
    
    for (int pos = 1; pos <= text_len; ++pos) {
        if (!occ_by_start[pos].empty()) {
            sort(occ_by_start[pos].begin(), occ_by_start[pos].end());
            for (int p_idx : occ_by_start[pos]) {
                cout << pos << ' ' << p_idx << '\n';
            }
        }
    }

    debug << "\n\n=============== РЕЗУЛЬТАТ ================\n";
    bool first = true;
    for (int pos = 1; pos <= text_len; ++pos) {
        if (!occ_by_start[pos].empty()) {
            sort(occ_by_start[pos].begin(), occ_by_start[pos].end());
            for (int p_idx : occ_by_start[pos]) {
                if (first) {
                    debug << "Найденные вхождения (позиция, образец): ";
                    first = false;
                }
                debug << "(" << pos << ", " << p_idx << ") ";
            }
        }
    }
    if (first) debug << "Вхождений не найдено.";
    debug << "\n";

    debug << "\n\n============ ИНДИВИДУАЛИЗАЦИЯ ============\n";
    debug << "Количество вершин в автомате: " << trie.size() << "\n";
    
    struct Entry {
        int start, end, pat;
    };
    vector<Entry> entries;
    for (int pos = 1; pos <= text_len; ++pos) {
        for (int p_idx : occ_by_start[pos]) {
            entries.push_back({pos, pos + pattern_len[p_idx] - 1, p_idx});
        }
    }
    
    set<int> overlapping;
    if (!entries.empty()) {
        sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
            return a.start < b.start;
        });
        for (size_t i = 0; i < entries.size(); ++i) {
            for (size_t j = i + 1; j < entries.size() && entries[j].start <= entries[i].end; ++j) {
                if (entries[i].pat != entries[j].pat) {
                    overlapping.insert(entries[i].pat);
                    overlapping.insert(entries[j].pat);
                }
            }
        }
    }
    
    if (overlapping.empty()) {
        debug << "Образцы с пересечениями с другими образцами: отсутствуют\n";
    } else {
        debug << "Образцы с пересечениями с другими образцами:";
        for (int p : overlapping) debug << " " << p;
        debug << "\n";
    }
    
    cout << debug.str();
    
    return 0;
}