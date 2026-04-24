#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int replace_cost, insert_cost, delete_cost;
    cin >> replace_cost >> insert_cost >> delete_cost;
    
    string A, B;
    cin >> A >> B;
    
    int n = A.size();
    int m = B.size();
    
    vector<vector<int>> dp(n + 1, vector<int>(m + 1));
    
    for (int i = 0; i <= n; ++i) {
        dp[i][0] = i * delete_cost;
    }
    for (int j = 0; j <= m; ++j) {
        dp[0][j] = j * insert_cost;
    }
    
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (A[i - 1] == B[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                int replace = dp[i - 1][j - 1] + replace_cost;
                int insert  = dp[i][j - 1] + insert_cost;
                int del     = dp[i - 1][j] + delete_cost;
                dp[i][j] = min({replace, insert, del});
            }
        }
    }
    
    cout << dp[n][m] << endl;
    return 0;
}