// Q3A_NeverAgainMLE.cpp
// Compile: g++ -std=c++17 Q3A_NeverAgainMLE.cpp -O2 -o q3a

#include <bits/stdc++.h>
using namespace std;

/*
  Compact open-addressing hash set holding 128-bit fingerprints (two uint64_t).
  Uses linear probing and resizes when load factor > 0.6.
*/

struct Finger128 {
    unsigned long long a;
    unsigned long long b;
    Finger128(): a(0), b(0) {}
    Finger128(unsigned long long _a, unsigned long long _b): a(_a), b(_b) {}
};

struct CompactSet128 {
    // table holds pairs (a,b).  a==0 && b==0 indicates empty slot.
    vector<unsigned long long> A; // stores a
    vector<unsigned long long> B; // stores b
    size_t capacity;
    size_t used;
    CompactSet128(size_t cap = 1<<20) { // default ~1M slots
        capacity = 1;
        while (capacity < cap) capacity <<= 1;
        A.assign(capacity, 0ULL);
        B.assign(capacity, 0ULL);
        used = 0;
    }

    static inline unsigned long long mix64(unsigned long long x) {
        // splitmix64
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        x = x ^ (x >> 31);
        return x;
    }

    void maybe_grow() {
        if ((double)used / (double)capacity > 0.58) {
            // rehash to double capacity
            size_t newcap = capacity << 1;
            vector<unsigned long long> nA(newcap, 0ULL), nB(newcap, 0ULL);
            for (size_t i = 0; i < capacity; ++i) {
                if (A[i] != 0ULL || B[i] != 0ULL) {
                    unsigned long long a = A[i], b = B[i];
                    // insert into nA/nB
                    unsigned long long h = (a ^ mix64(b)) & (newcap - 1);
                    while (nA[h] != 0ULL || nB[h] != 0ULL) {
                        h = (h + 1) & (newcap - 1);
                    }
                    nA[h] = a; nB[h] = b;
                }
            }
            A.swap(nA); B.swap(nB);
            capacity = newcap;
        }
    }

    bool contains(const Finger128 &f) const {
        if (f.a == 0ULL && f.b == 0ULL) {
            // avoid sentinel (rare): treat as not present
            // but we will never insert sentinel, we will remap it if needed before insert.
        }
        unsigned long long h = (f.a ^ mix64(f.b)) & (capacity - 1);
        while (true) {
            if (A[h] == 0ULL && B[h] == 0ULL) return false;
            if (A[h] == f.a && B[h] == f.b) return true;
            h = (h + 1) & (capacity - 1);
        }
        return false;
    }

    bool insert(const Finger128 &f) {
        // returns true if inserted now (not already present)
        // We never insert (0,0). If f == (0,0) remap it to (1, f.b) (extremely unlikely)
        Finger128 ff = f;
        if (ff.a == 0ULL && ff.b == 0ULL) ff.a = 1ULL;
        if (contains(ff)) return false;
        maybe_grow();
        unsigned long long h = (ff.a ^ mix64(ff.b)) & (capacity - 1);
        while (A[h] != 0ULL || B[h] != 0ULL) {
            h = (h + 1) & (capacity - 1);
        }
        A[h] = ff.a; B[h] = ff.b; ++used;
        return true;
    }
};

// Rolling-hash (two different bases) -> two 64-bit values
pair<unsigned long long, unsigned long long> fingerprint_of(const string &s) {
    const unsigned long long base1 = 1315423911ULL; // arbitrary high base
    const unsigned long long base2 = 11400714819323198485ULL; // odd number
    unsigned long long h1 = 1469598103934665603ULL; // FNV offset
    unsigned long long h2 = 1099511628211ULL; // FNV prime start value
    for (char ch : s) {
        unsigned long long x = (unsigned char)ch;
        h1 = h1 * base1 + x + 0x9e3779b97f4a7c15ULL;
        h2 = (h2 ^ x) * base2 + 0xC2B2AE3D27D4EB4FULL;
    }
    // final mix
    unsigned long long a = h1 ^ (h1 >> 32);
    unsigned long long b = h2 ^ (h2 >> 33);
    if (a == 0ULL && b == 0ULL) { a = 1ULL; } // avoid sentinel
    return {a, b};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n; cin >> n;
        // initialize compact set with conservative capacity
        CompactSet128 set(max(1, min(n * 2, 1<<20))); // min capacity dictates initial table size; cap 1<<20 if n too big
        for (int i = 0; i < n; ++i) {
            string s; cin >> s;
            auto pr = fingerprint_of(s);
            Finger128 f(pr.first, pr.second);
            bool already = set.contains(f);
            if (already) {
                cout << "1\n";
            } else {
                set.insert(f);
                cout << "0\n";
            }
        }
    }
    return 0;
}
