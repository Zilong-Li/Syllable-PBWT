#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <assert.h>
#include <vector>
#include <chrono>

using namespace std;

int main(int argc, char** argv) {
        ios_base::sync_with_stdio(0); cin.tie(0);
	string chr = argv[2];
	string vcf = "/data6/victor/panel" + chr + ".vcf";
        ifstream in(vcf);

        string line;
        while (getline(in, line)) {
                if (line[0] != '#' || line[1] != '#') break;
        }
        stringstream ss(line);
        int M = -9;
        while (getline(ss, line, '\t')) M++;
        M <<= 1;
	int L = stoi(argv[1]), N = 0;

	vector<vector<bool>> x;
	vector<vector<int>> a, d, u, v;
	vector<int> a0(M), a1(M), d0(M), d1(M);

        while (getline(in, line)) {
                ss = stringstream(line);
                for (int _ = 0; _ < 9; _++) getline(ss, line, '\t');
		x.push_back(vector<bool>(M));
		int index = 0;
		while (getline(ss, line, '\t')) {
			x[N][index++] = line[0] != '0';
			x[N][index++] = line[2] != '0';
		}

                int u_ = 0, v_ = 0, p = N + 1, q = N + 1;

		a.push_back(vector<int>(M));
		d.push_back(vector<int>(M));
		u.push_back(vector<int>(M + 1));
		v.push_back(vector<int>(M + 1));
                for (int i = 0; i < M; i++) {
			int d_ = N > 0 ? d[N - 1][i] : 0;
			int a_ = N > 0 ? a[N - 1][i] : i;
                        p = max(p, d_);
                        q = max(q, d_);
			u[N][i] = u_;
			v[N][i] = v_;

                        if (x[N][a_]) {
                                a1[v_] = a_;
                                d1[v_] = q;
                                v_++;
                                q = 0;
                        } else {
                                a0[u_] = a_;
                                d0[u_] = p;
                                u_++;
                                p = 0;
                        }
                }
		u[N][M] = u_;
		v[N][M] = M;

                for (int i = 0; i < M; i++) {
			v[N][i] += u_;
                        if (i < u_) {
                                a[N][i] = a0[i];
                                d[N][i] = d0[i];
                        } else {
                                a[N][i] = a1[i - u_];
                                d[N][i] = d1[i - u_];
                        }
                }
                N++;
        }

	cout << "fullmem " << M << ' ' << N << ' ' << L << endl;

	vector<int> t(N), zd(N), bd(N), dz(M);
	vector<vector<bool>> z;
	clock_t START = clock();
	int matches = 0;
	string q_vcf = "/data6/victor/query" + chr + ".vcf";
	ifstream q_in(q_vcf);
	while (getline(q_in, line)) {
		if (line[0] != '#' || line[1] != '#') break;
	}
	int Q = -9;
	ss = stringstream(line);
	while (getline(ss, line, '\t')) Q++;
	Q <<= 1;
	z.resize(N, vector<bool>(Q));
	int NN = 0;
	while (getline(q_in, line)) {
		ss = stringstream(line);
		for (int _ = 0; _ < 9; _++) getline(ss, line, '\t');
		int index = 0;
		while (getline(ss, line, '\t')) {
			z[NN][index++] = line[0] != '0';
			z[NN][index++] = line[2] != '0';
		}
		NN++;
	}

	for (int q = 0; q < Q; q++) {
	ofstream out("/home/vwang1/PBWT_compression/queryf-" + to_string(q) + ".out");
	t[0] = z[0][q] ? M : 0;
	for (int i = 1; i < N; i++) {
		if (z[i][q]) {
			t[i] = v[i][t[i - 1]];
		} else {
			t[i] = u[i][t[i - 1]];
		}
	}

	int z_idx = N, b_idx = N;
	for (int i = N - 1; i > -1; i--) {
		z_idx = min(z_idx, i + 1);
		b_idx = min(b_idx, i + 1);
		if (t[i] > 0) {
			while (z_idx > 0 && z[z_idx - 1][q] == x[z_idx - 1][a[i][t[i] - 1]]) z_idx--;
			zd[i] = z_idx;
		} else zd[i] = i + 1;
		if (t[i] < M) {
			while (b_idx > 0 && z[b_idx - 1][q] == x[b_idx - 1][a[i][t[i]]]) b_idx--;
			bd[i] = b_idx;
		} else bd[i] = i + 1;
	}

	int f = t[L - 2], g = t[L - 2], f_end, g_end;
	for (int i = L - 1; i < N; i++) {
		if (z[i][q]) {
			f_end = u[i][f];
			g_end = u[i][g];
			f = v[i][f];
			g = v[i][g];
		} else {
			f_end = v[i][f];
			g_end = v[i][g];
			f = u[i][f];
			g = u[i][g];
		}

		while (f_end < g_end) {
			matches++;
			out << dz[a[i][f_end]] << ' ' << i << ' ' << a[i][f_end] << '\n';
			f_end++;
		}

		if (f == g) {
			if (f > 0 && zd[i] == i + 1 - L) {
				dz[a[i][--f]] = i + 1 - L;
			}
			if (g < M && bd[i] == i + 1 - L) {
				dz[a[i][g++]] = i + 1 - L;
			}
		}

		if (f < g) {
			while (f > 0 && d[i][f] <= i + 1 - L) {
				dz[a[i][--f]] = i + 1 - L;
			}
			while (g < M && d[i][g] <= i + 1 - L) {
				dz[a[i][g++]] = i + 1 - L;
			}
		}
	}

	while (f < g) {
		matches++;
		out << dz[a[N - 1][f]] << ' ' << N << ' ' << a[N - 1][f] << '\n';
		f++;
	}
	}

	cout << clock() - START << " time\n";
	cout << matches << " matches\n\n";

	return 0;
}
