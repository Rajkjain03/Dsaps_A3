// Q2_SkipList_NoSTL_Final.cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <string>
using namespace std;

const int MAX_LEVEL = 16; // sufficient for 1e5 elements
const double P = 0.5;     // probability for random level

struct Node
{
    int val;
    Node **next;
    int level;
    Node(int v, int lvl)
    {
        val = v;
        level = lvl;
        next = new Node *[lvl];
        for (int i = 0; i < lvl; i++)
            next[i] = NULL;
    }
    ~Node() { delete[] next; }
};

class SkipList
{
    Node *head;
    int currLevel;

    int randomLevel()
    {
        int lvl = 1;
        while (((double)rand() / RAND_MAX) < P && lvl < MAX_LEVEL)
            lvl++;
        return lvl;
    }

public:
    SkipList()
    {
        srand((unsigned)time(NULL));
        currLevel = 1;
        head = new Node(INT_MIN, MAX_LEVEL);
    }

    ~SkipList()
    {
        Node *cur = head->next[0];
        while (cur)
        {
            Node *nxt = cur->next[0];
            delete cur;
            cur = nxt;
        }
        delete head;
    }

    void insert(int val)
    {
        Node *update[MAX_LEVEL];
        Node *x = head;

        for (int i = currLevel - 1; i >= 0; i--)
        {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
            update[i] = x;
        }

        int lvl = randomLevel();
        if (lvl > currLevel)
        {
            for (int i = currLevel; i < lvl; i++)
                update[i] = head;
            currLevel = lvl;
        }

        Node *n = new Node(val, lvl);
        for (int i = 0; i < lvl; i++)
        {
            n->next[i] = update[i]->next[i];
            update[i]->next[i] = n;
        }
    }

    void deleteAll(int val)
    {
        Node *update[MAX_LEVEL];
        Node *x = head;

        for (int i = currLevel - 1; i >= 0; i--)
        {
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
            update[i] = x;
        }
        x = x->next[0];

        // delete all nodes equal to val
        while (x && x->val == val)
        {
            for (int i = 0; i < x->level; i++)
            {
                if (update[i]->next[i] == x)
                    update[i]->next[i] = x->next[i];
            }
            Node *tmp = x;
            x = x->next[0];
            delete tmp;
        }

        // reduce list level if top levels empty
        while (currLevel > 1 && head->next[currLevel - 1] == NULL)
            currLevel--;
    }

    bool search(int val)
    {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--)
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
        x = x->next[0];
        return (x && x->val == val);
    }

    int countOccurrence(int val)
    {
        int cnt = 0;
        Node *x = head->next[0];
        while (x)
        {
            if (x->val == val)
                cnt++;
            else if (x->val > val)
                break;
            x = x->next[0];
        }
        return cnt;
    }

    int lowerBound(int val)
    {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--)
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];
        x = x->next[0];
        return x ? x->val : 0;
    }

    int upperBound(int val)
    {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--)
            while (x->next[i] && x->next[i]->val <= val)
                x = x->next[i];
        x = x->next[0];
        return x ? x->val : 0;
    }

    int closestElement(int val)
    {
        Node *x = head;
        for (int i = currLevel - 1; i >= 0; i--)
            while (x->next[i] && x->next[i]->val < val)
                x = x->next[i];

        Node *pred = (x == head ? NULL : x);
        Node *succ = x->next[0];

        if (succ && succ->val == val)
            return succ->val;
        if (!pred && !succ)
            return 0;
        if (!pred)
            return succ->val;
        if (!succ)
            return pred->val;

        long long d1 = (long long)val - pred->val;
        if (d1 < 0)
            d1 = -d1;
        long long d2 = (long long)succ->val - val;
        if (d2 < 0)
            d2 = -d2;
        return (d1 <= d2 ? pred->val : succ->val);
    }

    void printBottom()
    {
        Node *x = head->next[0];
        if (!x)
        {
            cout << "\n";
            return;
        }
        cout << x->val;
        x = x->next[0];
        while (x)
        {
            cout << " " << x->val;
            x = x->next[0];
        }
        cout << "\n";
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;
    SkipList sl;

    for (int i = 0; i < Q; i++)
    {
        string op;
        cin >> op;
        if (op == "insert")
        {
            int x;
            cin >> x;
            sl.insert(x);
            sl.printBottom();
        }
        else if (op == "delete")
        {
            int x;
            cin >> x;
            sl.deleteAll(x);
            sl.printBottom();
        }
        else if (op == "search")
        {
            int x;
            cin >> x;
            cout << (sl.search(x) ? "true" : "false") << "\n";
            sl.printBottom();
        }
        else if (op == "count")
        {
            int x;
            cin >> x;
            cout << sl.countOccurrence(x) << "\n";
            sl.printBottom();
        }
        else if (op == "lower")
        {
            int x;
            cin >> x;
            cout << sl.lowerBound(x) << "\n";
            sl.printBottom();
        }
        else if (op == "upper")
        {
            int x;
            cin >> x;
            cout << sl.upperBound(x) << "\n";
            sl.printBottom();
        }
        else if (op == "closest")
        {
            int x;
            cin >> x;
            cout << sl.closestElement(x) << "\n";
            sl.printBottom();
        }
    }
    return 0;
}
