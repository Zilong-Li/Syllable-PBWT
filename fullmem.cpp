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
        string vcf = "/data6/ukbiobank/genotype_data/autosomal_chroms_june2018/21981/ukb_hap_GP_removed/ukb_hap_chr21_v2.vcf";
        ifstream in(vcf);
	ofstream out("/home/vwang1/outo.txt");

        string line;
        while (getline(in, line)) {
                if (line[0] != '#' || line[1] != '#') break;
        }
        stringstream ss(line);
        int m = -9;
        while (getline(ss, line, '\t')) m++;
        m <<= 1;
        m--;
	int L = stoi(argv[1]), n = 0;

	vector<vector<bool>> x;
	vector<bool> z;
	vector<vector<int>> a, d, u, v;
	vector<int> a0(m), a1(m), d0(m), d1(m);

        while (getline(in, line)) {
                ss = stringstream(line);
                for (int _ = 0; _ < 9; _++) getline(ss, line, '\t');
                int index = 0;
		x.push_back(vector<bool>(m));
                while (getline(ss, line, '\t')) {
                        if (index == 0) {
				z.push_back(line[0] != '0');
                        } else {
                                x[n][index++] = line[0] != '0';
                        }
                        x[n][index++] = line[2] != '0';
                }

                assert(index == m);
                int u_ = 0, v_ = 0, p = n + 1, q = n + 1;

		a.push_back(vector<int>(m));
		d.push_back(vector<int>(m));
		u.push_back(vector<int>(m + 1));
		v.push_back(vector<int>(m + 1));
                for (int i = 0; i < m; i++) {
			int d_ = n > 0 ? d[n - 1][i] : 0;
			int a_ = n > 0 ? a[n - 1][i] : i;
                        p = max(p, d_);
                        q = max(q, d_);
			u[n][i] = u_;
			v[n][i] = v_;

                        if (x[n][a_]) {
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
		u[n][m] = u_;
		v[n][m] = m;

                for (int i = 0; i < m; i++) {
			v[n][i] += u_;
                        if (i < u_) {
                                a[n][i] = a0[i];
                                d[n][i] = d0[i];
                        } else {
                                a[n][i] = a1[i - u_];
                                d[n][i] = d1[i - u_];
                        }
                }
                n++;
        }

	vector<int> t(n), zd(n), bd(n), dz(m);
	clock_t START = clock();
	if (z[0]) t[0] = m;
	for (int i = 1; i < n; i++) {
		if (z[i]) {
			t[i] = v[i][t[i - 1]];
		} else {
			t[i] = u[i][t[i - 1]];
		}
	}

	int z_idx = n, b_idx = n;
	for (int i = n - 1; i > -1; i--) {
		z_idx = min(z_idx, i + 1);
		b_idx = min(b_idx, i + 1);
		if (t[i] > 0) {
			while (z_idx > 0 && z[z_idx - 1] == x[z_idx - 1][a[i][t[i] - 1]]) z_idx--;
			zd[i] = z_idx;
		} else zd[i] = i + 1;
		if (t[i] < m) {
			while (b_idx > 0 && z[b_idx - 1] == x[b_idx - 1][a[i][t[i]]]) b_idx--;
			bd[i] = b_idx;
		} else bd[i] = i + 1;
	}

	int f = t[L - 2], g = t[L - 2];
	int matches = 0;
	for (int i = L - 1; i < n; i++) {
		int f_end = z[i] ? u[i][f] : v[i][f];
		int g_end = z[i] ? u[i][g] : v[i][g];
		f = z[i] ? v[i][f] : u[i][f];
		g = z[i] ? v[i][g] : u[i][g];

		while (f_end < g_end) {
			matches++;
			out << dz[a[i][f_end]] << ' ' << i << ' ' << a[i][f_end] << '\n';
			f_end++;
		}

		if (f == g) {
			if (f > 0 && zd[i] == i + 1 - L) {
				f--;
				dz[a[i][f]] = i + 1 - L;
			}
			if (g < m && bd[i] == i + 1 - L) {
				dz[a[i][g]] = i + 1 - L;
				g++;
			}
		}

		if (f < g) {
			while (f > 0 && d[i][f] <= i + 1 - L) {
				f--;
				dz[a[i][f]] = i + 1 - L;
			}
			while (g < m && d[i][g] <= i + 1 - L) {
				dz[a[i][g]] = i + 1 - L;
				g++;
			}
		}
	}

	while (f < g) {
		matches++;
		out << dz[a[n - 1][f]] << ' ' << n << ' ' << a[n - 1][f] << '\n';
		f++;
	}

	cout << clock() - START << " time\n";
	cout << "fullmem " << m << ' ' << n << ' ' << L << endl;
	cout << matches << " matches\n";

	return 0;
}
