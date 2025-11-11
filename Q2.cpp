#include <iostream>
using namespace std;

const int MAX_LEVEL = 16;
const double P = 0.5;
unsigned long long rng_seed = 88172645463325252ULL;

unsigned long long nextRand() {
    rng_seed ^= rng_seed << 13;
    rng_seed ^= rng_seed >> 7;
    rng_seed ^= rng_seed << 17;
    return rng_seed;
}

// --- Node Definition ---
struct Node {
    int val;
    Node **next;
    int level;
    
    Node(int v, int lvl) {
        val = v;
        level = lvl;
        next = new Node*[lvl];
        for (int i = 0; i < lvl; i++)
            next[i] = NULL;
    }

    ~Node() {
        delete[] next;
    }
};

// --- SkipList Class ---
class SkipList {
    Node *head;
    int currLevel;

    int randomLevel() {
        int lvl = 1;
        while ((nextRand() & 1) && lvl < MAX_LEVEL)
            lvl++;
        return lvl;
    }

public:
    SkipList() {
        currLevel = 1;
        head = new Node(-1000000000, MAX_LEVEL); // sentinel node
    }

    ~SkipList() {
        Node *cur = head->next[0];
        while (cur) {
            Node *nxt = cur->next[0];
            delete cur;
            cur = nxt;
        }
        delete head;
    }

    // --- Insert operation ---
    void insert(int val) {
        Node *update[MAX_LEVEL];
        Node *x = head;

        for (int i = currLevel - 1; i >= 0; i--) {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
            update[i] = x;
        }

        int lvl = randomLevel();
        if (lvl > currLevel) {
            for (int i = currLevel; i < lvl; i++)
                update[i] = head;
            currLevel = lvl;
        }

        Node *n = new Node(val, lvl);
        for (int i = 0; i < lvl; i++) {
            n->next[i] = update[i]->next[i];
            update[i]->next[i] = n;
        }
    }

    // --- Delete All occurrences of a value ---
    void deleteAll(int val) {
        Node *update[MAX_LEVEL];
        Node *x = head;

        for (int i = currLevel - 1; i >= 0; i--) {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
            update[i] = x;
        }

        x = x->next[0];
        bool deleted = false;
        while (x && x->val == val) {
            for (int i = 0; i < x->level; i++) {
                if (update[i]->next[i] == x)
                    update[i]->next[i] = x->next[i];
            }
            Node *tmp = x;
            x = x->next[0];
            delete tmp;
            deleted = true;
        }

        if (deleted) {
            while (currLevel > 1 && head->next[currLevel - 1] == NULL)
                currLevel--;
        }
    }

    // --- Search value ---
    bool search(int val) {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--) {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
        }
        x = x->next[0];
        return (x && x->val == val);
    }

    // --- Count Occurrence ---
    int countOccurrence(int val) {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--) {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
        }
        x = x->next[0];
        int cnt = 0;
        while (x && x->val == val) {
            cnt++;
            x = x->next[0];
        }
        return cnt;
    }

    // --- Lower Bound ---
    int lowerBound(int val) {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--) {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
        }
        x = x->next[0];
        return x ? x->val : 0;
    }

    // --- Upper Bound ---
    int upperBound(int val) {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--) {
            while (x->next[i] && x->next[i]->val <= val)
                x = x->next[i];
        }
        x = x->next[0];
        return x ? x->val : 0;
    }

    // --- Closest Element ---
    int closestElement(int val) {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--) {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
        }

        Node *pred = (x == head ? NULL : x);
        Node *succ = x->next[0];

        if (succ && succ->val == val) return succ->val;
        if (!pred && !succ) return 0;
        if (!pred) return succ->val;
        if (!succ) return pred->val;

        long long d1 = (long long)val - pred->val;
        if (d1 < 0) d1 = -d1;
        long long d2 = (long long)succ->val - val;
        if (d2 < 0) d2 = -d2;
        return (d1 <= d2 ? pred->val : succ->val);
    }

    // --- Print the base level of skip list ---
    void printBottom() {
        Node *x = head->next[0];
        if (!x) {
            cout << "\n";
            return;
        }
        cout << x->val;
        x = x->next[0];
        while (x) {
            cout << " " << x->val;
            x = x->next[0];
        }
        cout << "\n";
    }
};

// --- Main Function (as per PDF format) ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    SkipList sl;
    cout <<endl;
    while (N-- > 0) {
        int choice;
        cin >> choice;


        if (choice == 1) {          // insert
            int x; cin >> x;
            sl.insert(x);
            sl.printBottom();
        } 
        else if (choice == 2) {     // delete
            int x; cin >> x;
            sl.deleteAll(x);
            sl.printBottom();
        } 
        else if (choice == 3) {     // search
            int x; cin >> x;
            cout << (sl.search(x) ? "true" : "false") << endl;
            sl.printBottom();
        } 
        else if (choice == 4) {     // count occurrences
            int x; cin >> x;
            cout << sl.countOccurrence(x) << endl;
            sl.printBottom();
        } 
        else if (choice == 5) {     // lower bound
            int x; cin >> x;
            cout << sl.lowerBound(x) << endl;
            sl.printBottom();
        } 
        else if (choice == 6) {     // upper bound
            int x; cin >> x;
            cout << sl.upperBound(x) << endl;
            sl.printBottom();
        } 
        else if (choice == 7) {     // closest element
            int x; cin >> x;
            cout << sl.closestElement(x) << endl;
            sl.printBottom();
        } 
        else {
            break;
        }
    }

    return 0;
}
