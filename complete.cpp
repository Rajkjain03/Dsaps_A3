#include <iostream>
#include <string>
#include <vector>
using namespace std;

/* ---------------------------------------------------------
Q3A — Never Again MLE (Bloom Filter)
--------------------------------------------------------- */
class BloomFilter {
    vector<unsigned char> bits;
    long long size;
    int k;

    // hash1 (djb2)
    unsigned long long hash1(const string &s) {
        unsigned long long h = 5381;
        for (char c : s) h = ((h << 5) + h) + c;
        return h;
    }

    // hash2 (FNV-1a)
    unsigned long long hash2(const string &s) {
        unsigned long long h = 1469598103934665603ULL;
        for (char c : s) {
            h ^= (unsigned long long)c;
            h *= 1099511628211ULL;
        }
        return h;
    }

public:
    BloomFilter(long long sz = 8LL * 1024 * 1024 * 8, int k_ = 3) {
        size = sz;
        bits.assign(size / 8, 0);
        k = k_;
    }

    // --- OPTIMIZATION 1: Added clear() method ---
    void clear() {
        bits.assign(size / 8, 0);
    }

    void add(const string &s) {
        unsigned long long h1v = hash1(s);
        unsigned long long h2v = hash2(s);
        for (int i = 0; i < k; i++) {
            unsigned long long pos = (h1v + i * h2v) % size;
            bits[pos / 8] |= (1 << (pos % 8));
        }
    }

    bool possiblyExists(const string &s) {
        unsigned long long h1v = hash1(s);
        unsigned long long h2v = hash2(s);
        for (int i = 0; i < k; i++) {
            unsigned long long pos = (h1v + i * h2v) % size;
            if (!(bits[pos / 8] & (1 << (pos % 8)))) return false;
        }
        return true;
    }
};

/* ---------------------------------------------------------
Q3B — Pooling Resources (Compliant Solution)
(Classes are unchanged)
--------------------------------------------------------- */

// --- Hash function for strings ---
unsigned long long hashString(const string &s) {
    unsigned long long h = 5381;
    for (char c : s) h = ((h << 5) + h) + c;
    return h;
}

// --- Hash function for long long keys ---
unsigned long long hashLong(long long k) {
    k ^= k >> 33;
    k *= 1099511628211ULL;
    k ^= k >> 33;
    return k;
}

// --- String Pool: Maps string -> int (pool_id) ---
struct StringPoolEntry {
    int pool_id = -1; 
    bool used = false;
};

class StringPool {
    vector<StringPoolEntry> table;
    int capacity;
public:
    StringPool(int cap = 100003) {
        capacity = cap;
        table.resize(capacity);
    }
    int findOrAdd(const string &val, vector<string> &stringStorage) {
        unsigned long long idx = hashString(val) % capacity;
        for (int i = 0; i < capacity; i++) {
            unsigned long long j = (idx + i) % capacity;
            if (!table[j].used) {
                stringStorage.push_back(val);
                int new_id = stringStorage.size() - 1;
                table[j].used = true;
                table[j].pool_id = new_id;
                return new_id;
            }
            if (stringStorage[table[j].pool_id] == val) {
                return table[j].pool_id;
            }
        }
        return -1; 
    }
};

// --- IntKey Map: Maps long long (key) -> int (pool_id) ---
struct IntKeyEntry {
    long long key;
    int pool_id = -1;
    bool used = false;
};

class IntKeyMap {
    vector<IntKeyEntry> table;
    int capacity;
public:
    IntKeyMap(int cap = 100003) {
        capacity = cap;
        table.resize(capacity);
    }
    void put(long long key, int pool_id) {
        unsigned long long idx = hashLong(key) % capacity;
        for (int i = 0; i < capacity; i++) {
            unsigned long long j = (idx + i) % capacity;
            if (!table[j].used || table[j].key == key) {
                table[j].key = key;
                table[j].pool_id = pool_id;
                table[j].used = true;
                return;
            }
        }
    }
    int get(long long key) {
        unsigned long long idx = hashLong(key) % capacity;
        for (int i = 0; i < capacity; i++) {
            unsigned long long j = (idx + i) % capacity;
            if (!table[j].used) return -1;
            if (table[j].key == key) return table[j].pool_id;
        }
        return -1;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    BloomFilter bf;

    while (t--) {
        int type;
        cin >> type;

        if (type == 1) {
            // Q3A: Never Again MLE
            int n;
            cin >> n;
            
            // --- OPTIMIZATION 1: Clear filter for new test case ---
            bf.clear(); 
            
            for (int i = 0; i < n; i++) {
                string s;
                cin >> s;
                bool seen = bf.possiblyExists(s);
                cout << (seen ? "1\n" : "0\n");
                if (!seen) bf.add(s);
            }
        } else if (type == 2) {
            // Q3B: Pooling Resources
            int n;
            cin >> n;
            
            // Note: We're keeping the dynamic capacity to handle
            // n > 200003, which is allowed by the constraints.
            int map_capacity = n * 2 + 7; 
            
            vector<string> stringStorage;
            
            // --- OPTIMIZATION 2: Reserve capacity ---
            stringStorage.reserve(n);
            
            StringPool stringPool(map_capacity);
            IntKeyMap intKeyMap(map_capacity);

            for (int i = 0; i < n; i++) {
                int qtype;
                cin >> qtype;
                if (qtype == 0) { // put
                    long long id;
                    string s;
                    cin >> id >> s;
                    int pool_id = stringPool.findOrAdd(s, stringStorage);
                    intKeyMap.put(id, pool_id);
                } else { // get
                    long long id;
                    cin >> id;
                    int pool_id = intKeyMap.get(id);
                    if (pool_id == -1) {
                        cout << "0\n";
                    } else {
                        cout << stringStorage[pool_id] << "\n";
                    }
                }
            }
        }
    }
    return 0;
}