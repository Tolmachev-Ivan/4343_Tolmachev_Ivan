#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <string>
#include <iomanip>
#include <stdexcept>

using namespace std;

static void print_usage() {
    cerr << "Usage:\n";
    cerr << "  MST-2approx: 0 n mode min_w max_w start_vertex [output_file]\n";
    cerr << "  Little: 1 n mode min_w max_w [output_file]\n";
    cerr << "  mode: 0 = random asymmetric matrix\n";
    cerr << "        1 = symmetric matrix\n";
}

int main(int argc, char* argv[]) {
    if (argc < 6) {
        print_usage();
        return 1;
    }

    int alg = 0;
    int n = 0;
    int mode = 0;
    int min_w = 0;
    int max_w = 0;
    int start_vertex = 0;

    try {
        alg = stoi(argv[1]);
        n = stoi(argv[2]);
        mode = stoi(argv[3]);
        min_w = stoi(argv[4]);
        max_w = stoi(argv[5]);
    } catch (...) {
        print_usage();
        return 1;
    }

    if (alg != 0 && alg != 1) {
        print_usage();
        return 1;
    }
    if (n <= 0 || min_w > max_w || (mode != 0 && mode != 1)) {
        print_usage();
        return 1;
    }

    ostream* out = &cout;
    ofstream fout;

    if (alg == 1) {
        if (argc >= 7) {
            fout.open(argv[6]);
            if (!fout) {
                cerr << "Cannot open output file.\n";
                return 1;
            }
            out = &fout;
        }
    } else {
        if (argc < 7) {
            print_usage();
            return 1;
        }
        try {
            start_vertex = stoi(argv[6]);
        } catch (...) {
            print_usage();
            return 1;
        }
        if (start_vertex < 0 || start_vertex >= n) {
            cerr << "start_vertex must be in [0, n-1]\n";
            return 1;
        }

        if (argc >= 8) {
            fout.open(argv[7]);
            if (!fout) {
                cerr << "Cannot open output file.\n";
                return 1;
            }
            out = &fout;
        }
    }

    random_device rd;
    mt19937 rng(rd());

    if (alg == 1) {
        uniform_int_distribution<int> dist(min_w, max_w);
        vector<vector<int>> w(n, vector<int>(n, -1));

        if (mode == 1) {
            for (int i = 0; i < n; ++i) {
                w[i][i] = -1;
                for (int j = i + 1; j < n; ++j) {
                    int val = dist(rng);
                    w[i][j] = w[j][i] = val;
                }
            }
        } else {
            for (int i = 0; i < n; ++i) {
                w[i][i] = -1;
                for (int j = 0; j < n; ++j) {
                    if (i != j) {
                        w[i][j] = dist(rng);
                    }
                }
            }
        }

        *out << n << '\n';
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (j) *out << ' ';
                *out << w[i][j];
            }
            *out << '\n';
        }
    } else {
        uniform_real_distribution<double> dist(static_cast<double>(min_w),
                                                static_cast<double>(max_w));
        vector<vector<double>> w(n, vector<double>(n, -1.0));

        for (int i = 0; i < n; ++i) {
            w[i][i] = -1.0;
            for (int j = i + 1; j < n; ++j) {
                double val = dist(rng);
                w[i][j] = w[j][i] = val;
            }
        }

        *out << start_vertex << '\n';
        *out << fixed << setprecision(2);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (j) *out << ' ';
                if (i == j) {
                    *out << -1;
                } else {
                    *out << w[i][j];
                }
            }
            *out << '\n';
        }
    }

    return 0;
}