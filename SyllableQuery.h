#pragma once

#include "vcfpp.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using namespace std;

using IntMapU = unordered_map<int, int>;

template<class T>
struct SyllableQuery
{
    static const unsigned long long MOD{-1ull - 58}, BASE{(1ull << 48) - 59};
    const int B{sizeof(T) * 8};
    int M{0}, N{0}, n{0};
    int minSiteL{B * 2 - 1};
    vector<vector<int>> x, a;
    vector<vector<T>> r;
    vector<vector<unsigned long long>> h;
    vector<T> z_;
    vector<unsigned __int128> xp;
    vector<unsigned long long> hz;
    vector<int> z, up_end, dn_end;

    int precompute(std::string vcfpanel, std::string samples, std::string region);
    int save(const char * save_file);
    int load(const char * load_file);
    inline void refine(int s_idx, int e_idx, int xi, int * start, int * end);
    int query(const vector<T> & zg, const int L, IntMapU& haplens, IntMapU& hapends);
    vector<T> encode_zg(const vector<int> & iz);
};
