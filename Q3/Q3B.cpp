// Q3B_PoolingResources.cpp
// Compile: g++ -std=c++17 Q3B_PoolingResources.cpp -O2 -o q3b
// NOTE: Only uses <iostream>, <string>, <vector>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

/*
  Implements:
  - string pool: vector<string> pool (1-based indices)
  - fingerprint -> pool index map (compact open addressing)
  - key (unsigned long long) -> pool index map (compact open addressing)

  Keys i in input satisfy 1 <= i <= 1e18, so 0 is a safe sentinel for empty slots.
*/

typedef unsigned long long ull;

// small mix function (splitmix-like)
static inline ull mix64(ull x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

// Fingerprint generator (64-bit) (we will actually use two 64-bit by combining different seeds)
// but to keep memory low we use only one 64-bit fingerprint for pool equality test
// Given the problem constraints and moderate S (sum unique lengths <= 1e6), single 64-bit fingerprint is adequate.
ull fingerprint_of(const string &s, ull seed = 1469598103934665603ULL) {
    ull h = seed;
    for (char c : s) {
        h = (h * 1315423911ULL) ^ (unsigned char)c;
        h = mix64(h);
    }
    if (h == 0ULL) h = 1ULL;
    return h;
}

// compact map: fingerprint (ull) -> int value (pool index)
// open addressing linear probing
struct FPMap {
    vector<ull> keys;
    vector<int> vals;
    size_t cap;
    size_t filled;
    FPMap(size_t initial = 1 << 10) {
        cap = 1;
        while (cap < initial) cap <<= 1;
        keys.assign(cap, 0ULL);
        vals.assign(cap, 0);
        filled = 0;
    }
    void maybe_grow() {
        if ((double)filled / (double)cap > 0.58) {
            size_t ncap = cap << 1;
            vector<ull> nkeys(ncap, 0ULL);
            vector<int> nvals(ncap, 0);
            for (size_t i = 0; i < cap; ++i) {
                if (keys[i] != 0ULL) {
                    ull k = keys[i];
                    int v = vals[i];
                    size_t h = (size_t)(k & (ncap - 1));
                    while (nkeys[h] != 0ULL) h = (h + 1) & (ncap - 1);
                    nkeys[h] = k; nvals[h] = v;
                }
            }
            keys.swap(nkeys); vals.swap(nvals); cap = ncap;
        }
    }
    // get(value) returns poolIndex or 0 if not present
    int get(ull k) const {
        if (k == 0ULL) return 0;
        size_t h = (size_t)(k & (cap - 1));
        while (true) {
            if (keys[h] == 0ULL) return 0;
            if (keys[h] == k) return vals[h];
            h = (h + 1) & (cap - 1);
        }
    }
    // insert key with val; if key exists, update value; returns true if new insertion
    bool put(ull k, int v) {
        if (k == 0ULL) k = 1ULL;
        int existing = get(k);
        if (existing != 0) {
            // update
            size_t h = (size_t)(k & (cap - 1));
            while (keys[h] != k) h = (h + 1) & (cap - 1);
            vals[h] = v;
            return false;
        }
        maybe_grow();
        size_t h = (size_t)(k & (cap - 1));
        while (keys[h] != 0ULL) h = (h + 1) & (cap - 1);
        keys[h] = k; vals[h] = v; ++filled;
        return true;
    }
};

// key (ull) -> pool index map
struct KeyMap {
    vector<ull> keys;
    vector<int> vals;
    size_t cap;
    size_t filled;
    KeyMap(size_t initial = 1 << 10) {
        cap = 1;
        while (cap < initial) cap <<= 1;
        keys.assign(cap, 0ULL);
        vals.assign(cap, 0);
        filled = 0;
    }
    void maybe_grow() {
        if ((double)filled / (double)cap > 0.58) {
            size_t ncap = cap << 1;
            vector<ull> nkeys(ncap, 0ULL);
            vector<int> nvals(ncap, 0);
            for (size_t i = 0; i < cap; ++i) {
                if (keys[i] != 0ULL) {
                    ull k = keys[i];
                    int v = vals[i];
                    size_t h = (size_t)(k & (ncap - 1));
                    while (nkeys[h] != 0ULL) h = (h + 1) & (ncap - 1);
                    nkeys[h] = k; nvals[h] = v;
                }
            }
            keys.swap(nkeys); vals.swap(nvals); cap = ncap;
        }
    }
    int get(ull k) const {
        if (k == 0ULL) return 0;
        size_t h = (size_t)(k & (cap - 1));
        while (true) {
            if (keys[h] == 0ULL) return 0;
            if (keys[h] == k) return vals[h];
            h = (h + 1) & (cap - 1);
        }
    }
    // put: set or update mapping k -> v
    void put(ull k, int v) {
        if (k == 0ULL) k = 1ULL;
        int existing = get(k);
        if (existing != 0) {
            // update in-place
            size_t h = (size_t)(k & (cap - 1));
            while (keys[h] != k) h = (h + 1) & (cap - 1);
            vals[h] = v;
            return;
        }
        maybe_grow();
        size_t h = (size_t)(k & (cap - 1));
        while (keys[h] != 0ULL) h = (h + 1) & (cap - 1);
        keys[h] = k; vals[h] = v; ++filled;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n; cin >> n;
        vector<string> pool;
        pool.push_back(string()); // pool[0] unused; actual entries start at 1
        FPMap fpm(1024);
        KeyMap km(1024);
        for (int qi = 0; qi < n; ++qi) {
            int type; cin >> type;
            if (type == 0) {
                unsigned long long key; string s;
                cin >> key >> s;
                // find s in pool via fingerprint
                ull fp = fingerprint_of(s);
                int idx = fpm.get(fp);
                if (idx == 0) {
                    // need to insert new string to pool
                    pool.push_back(s);
                    idx = (int)pool.size() - 1;
                    fpm.put(fp, idx);
                } else {
                    // possible fp collision: double-check exact string matches
                    if (pool[idx] != s) {
                        // collision: linear linear-probing search among subsequent fp slots for matching string
                        // Insert new string as distinct
                        pool.push_back(s);
                        idx = (int)pool.size() - 1;
                        // but fingerprint equal to existing slot; to avoid clobber we'll adjust fp by mixing and reinsert
                        ull newfp = mix64(fp ^ (ull)idx);
                        while (fpm.get(newfp) != 0) newfp = mix64(newfp + 0x9e3779b97f4a7c15ULL);
                        fpm.put(newfp, idx);
                    }
                }
                // map integer key -> pool index
                km.put(key, idx);
            } else if (type == 1) {
                unsigned long long key; cin >> key;
                int idx = km.get(key);
                if (idx == 0) {
                    cout << "0\n";
                } else {
                    cout << pool[idx] << "\n";
                }
            } else {
                // unknown type: skip line
                string rest; getline(cin, rest);
            }
        }
    }
    return 0;
}
