#include <iostream>
#include <string>
#include <vector>
using namespace std;

using ull = unsigned long long;
using ll = long long;

class BloomFilter {
    vector<unsigned char> bits;  // bit array stored in vector
    long long size;              // number of bits
    int k;                       // number of hash functions

   ull polyHash(const string &s,ull base) {
       ull h = 0;
        for (char c : s)
            h = h * base + (unsigned long long)(unsigned char)c;
        return h;
    }

public:
    // Constructor (8 MB → 67,108,864 bits)
    BloomFilter(long long sz = 8LL * 1024 * 1024 * 8, int k_ = 3) {
        size = sz;
        bits.assign(size / 8, 0);
        k = k_;
    }

    // Clear all bits (for each test case)
    void clear() {
        for (size_t i = 0; i < bits.size(); i++)
            bits[i] = 0;
    }

    // Add string to Bloom Filter
    void add(const string &s) {
       ull h1 = polyHash(s, 131);
       ull h2 = polyHash(s, 137);

        for (int i = 0; i < k; i++) {
           ull pos = (h1 + i * h2) % size;
            bits[pos / 8] |= (1 << (pos % 8));
        }
    }

    // Check if string possibly exists
    bool possiblyExists(const string &s) {
       ull h1 = polyHash(s, 131);
       ull h2 = polyHash(s, 137);

        for (int i = 0; i < k; i++) {
           ull pos = (h1 + i * h2) % size;
            if (!(bits[pos / 8] & (1 << (pos % 8))))
                return false;
        }
        return true;
    }
};

// --- Main Function for Q3A ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    BloomFilter bf;  // one Bloom Filter reused

    while (t--) {
        int n;
        cin >> n;
        bf.clear();  // reset before each test case

        for (int i = 0; i < n; i++) {
            string s;
            cin >> s;
            bool seen = bf.possiblyExists(s);
            cout << (seen ? "1\n" : "0\n");
            if (!seen) bf.add(s);
        }
    }

    return 0;
}
