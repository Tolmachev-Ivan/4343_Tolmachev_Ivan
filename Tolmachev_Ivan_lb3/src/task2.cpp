#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    string S, T;
    cin >> S >> T;
    
    int n = S.size();
    int m = T.size();
    
    vector<int> prev(m + 1);
    vector<int> curr(m + 1);
    
    for (int j = 0; j <= m; ++j) {
        prev[j] = j;
    }
    
    for (int i = 1; i <= n; ++i) {
        curr[0] = i;
        for (int j = 1; j <= m; ++j) {
            if (S[i - 1] == T[j - 1]) {
                curr[j] = prev[j - 1];
            } else {
                int replace = prev[j - 1] + 1;
                int insert  = curr[j - 1] + 1;
                int del     = prev[j] + 1;
                curr[j] = min({replace, insert, del});
            }
        }
        swap(prev, curr);
    }
    
    cout << prev[m] << endl;
    return 0;
}