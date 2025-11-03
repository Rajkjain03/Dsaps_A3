// Q2_SkipList.cpp
// Compile: g++ -std=c++17 Q2_SkipList.cpp -O2 -o skiplist

#include <bits/stdc++.h>
using namespace std;

struct SkipNode {
    int val;
    vector<SkipNode*> forward;
    SkipNode(int v, int level) : val(v), forward(level, nullptr) {}
};

class SkipList {
private:
    int maxLevel;
    double p; // probability
    int level; // current highest level
    SkipNode* header;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;

    int randomLevel() {
        int lvl = 1;
        while (lvl < maxLevel && dist(rng) < p) lvl++;
        return lvl;
    }

public:
    SkipList(int maxLevel_ = 32, double p_ = 0.5)
        : maxLevel(maxLevel_), p(p_), level(1),
          rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count()),
          dist(0.0, 1.0) {
        header = new SkipNode(INT_MIN, maxLevel); // sentinel head
    }

    ~SkipList() {
        // delete all nodes
        SkipNode* cur = header;
        while (cur) {
            SkipNode* next = cur->forward[0];
            delete cur;
            cur = next;
        }
    }

    void insert(int value) {
        vector<SkipNode*> update(maxLevel, nullptr);
        SkipNode* x = header;
        // find update[]: rightmost node at each level < value
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && x->forward[i]->val < value) x = x->forward[i];
            update[i] = x;
        }
        // move to possible position at level 0
        x = x->forward[0];

        // Insert as new node with random level always (duplicates allowed)
        int rndLevel = randomLevel();
        if (rndLevel > level) {
            for (int i = level; i < rndLevel; ++i) update[i] = header;
            level = rndLevel;
        }
        SkipNode* node = new SkipNode(value, rndLevel);
        for (int i = 0; i < rndLevel; ++i) {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }
    }

    bool search(int value) const {
        SkipNode* x = header;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && x->forward[i]->val < value) x = x->forward[i];
        }
        x = x->forward[0];
        return x && x->val == value;
    }

    int count_occurrence(int value) const {
        int cnt = 0;
        SkipNode* x = header->forward[0];
        while (x) {
            if (x->val == value) ++cnt;
            else if (x->val > value) break;
            x = x->forward[0];
        }
        return cnt;
    }

    void delete_all(int value) {
        vector<SkipNode*> update(maxLevel, nullptr);
        SkipNode* x = header;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && x->forward[i]->val < value) x = x->forward[i];
            update[i] = x;
        }
        x = x->forward[0];
        // If the first occurrence exists, we need to remove nodes equal to value
        while (x && x->val == value) {
            // Remove x: for each level, if update[level].forward[level] == x then bypass
            for (int i = 0; i < (int)x->forward.size(); ++i) {
                if (update[i]->forward[i] == x) update[i]->forward[i] = x->forward[i];
            }
            SkipNode* toDel = x;
            x = x->forward[0];
            delete toDel;
            // After deleting one node we need to recompute update[] for the next equal node:
            // move forward pointers in update that still point to deleted node to point to the predecessor of x
            // Simplest: recompute update[] from header for remaining levels (since duplicates might be consecutive).
            // Efficient approach: move update pointers forward if they pointed to deleted node.
            for (int i = 0; i < level; ++i) {
                while (update[i]->forward[i] && update[i]->forward[i]->val < value) update[i] = update[i]->forward[i];
            }
        }
        // adjust level downward if necessary
        while (level > 1 && header->forward[level - 1] == nullptr) --level;
    }

    int lower_bound(int value) const {
        SkipNode* x = header;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && x->forward[i]->val < value) x = x->forward[i];
        }
        x = x->forward[0];
        if (!x) return 0;
        return x->val;
    }

    int upper_bound(int value) const {
        SkipNode* x = header;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && x->forward[i]->val <= value) x = x->forward[i];
        }
        x = x->forward[0];
        if (!x) return 0;
        return x->val;
    }

    int closest_element(int value) const {
        if (!header->forward[0]) return 0; // empty
        // find predecessor (last < =value) and successor (first > =value or >value)
        SkipNode* x = header;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && x->forward[i]->val < value) x = x->forward[i];
        }
        // x is last node with val < value or header
        SkipNode* pred = (x == header ? nullptr : x);
        SkipNode* succ = x->forward[0];
        // succ might be nullptr (end) or >= value
        if (succ && succ->val == value) {
            // exact match: return value (one of the duplicates)
            return succ->val;
        }
        // compute candidates
        long long cand1 = LLONG_MAX, cand2 = LLONG_MAX;
        if (pred) cand1 = llabs((long long)pred->val - value);
        if (succ) cand2 = llabs((long long)succ->val - value);
        if (pred == nullptr && succ == nullptr) return 0;
        if (pred == nullptr) return succ->val;
        if (succ == nullptr) return pred->val;
        if (cand1 <= cand2) return pred->val; // tie -> smaller (pred is smaller)
        else return succ->val;
    }

    // Return bottom level as vector<int>
    vector<int> get_bottom_elements() const {
        vector<int> out;
        SkipNode* cur = header->forward[0];
        while (cur) {
            out.push_back(cur->val);
            cur = cur->forward[0];
        }
        return out;
    }

    // Print bottom level on single line (space separated). If empty prints blank line.
    void print_bottom(ostream &os = cout) const {
        vector<int> arr = get_bottom_elements();
        if (arr.empty()) { os << "\n"; return; }
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i) os << " ";
            os << arr[i];
        }
        os << "\n";
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    if (!(cin >> Q)) return 0;
    SkipList sl;

    for (int i = 0; i < Q; ++i) {
        string cmd;
        if (!(cin >> cmd)) break;
        if (cmd == "insert") {
            int x; cin >> x;
            sl.insert(x);
            sl.print_bottom();
        } else if (cmd == "delete") {
            int x; cin >> x;
            sl.delete_all(x);
            sl.print_bottom();
        } else if (cmd == "search") {
            int x; cin >> x;
            bool f = sl.search(x);
            cout << (f ? "true" : "false") << "\n";
            sl.print_bottom();
        } else if (cmd == "count") {
            int x; cin >> x;
            cout << sl.count_occurrence(x) << "\n";
            sl.print_bottom();
        } else if (cmd == "lower") {
            int x; cin >> x;
            cout << sl.lower_bound(x) << "\n";
            sl.print_bottom();
        } else if (cmd == "upper") {
            int x; cin >> x;
            cout << sl.upper_bound(x) << "\n";
            sl.print_bottom();
        } else if (cmd == "closest") {
            int x; cin >> x;
            cout << sl.closest_element(x) << "\n";
            sl.print_bottom();
        } else if (cmd == "print") {
            sl.print_bottom();
        } else {
            // unknown — ignore remaining line
            string rest;
            getline(cin, rest);
            sl.print_bottom();
        }
    }
    return 0;
}
