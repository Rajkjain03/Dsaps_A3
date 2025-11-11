#include <iostream>
#include <string>
using namespace std;

string stripLeadingZeros(string s)
{
    int i = 0;
    while (i < (int)s.length() - 1 && s[i] == '0')
        i++;
    return s.substr(i);
}

int compareStrings(const string &a, const string &b)
{
    string A = stripLeadingZeros(a);
    string B = stripLeadingZeros(b);
    if (A.length() < B.length())
        return -1;
    if (A.length() > B.length())
        return 1;
    for (int i = 0; i < (int)A.length(); i++)
    {
        if (A[i] < B[i]) return -1;
        if (A[i] > B[i]) return 1;
    }
    return 0;
}

// ---------- Addition ----------
string addStrings(string a, string b)
{
    int i = a.length() - 1, j = b.length() - 1, carry = 0;
    string res = "";
    while (i >= 0 || j >= 0 || carry)
    {
        int da = (i >= 0) ? a[i--] - '0' : 0;
        int db = (j >= 0) ? b[j--] - '0' : 0;
        int sum = da + db + carry;
        res = char('0' + (sum % 10)) + res;
        carry = sum / 10;
    }
    return stripLeadingZeros(res);
}

// ---------- Subtraction (a >= b only, per spec) ----------
string subtractStrings(string a, string b)
{
    if (compareStrings(a, b) < 0) return "0"; // no negatives allowed
    int i = a.length() - 1, j = b.length() - 1, borrow = 0;
    string res = "";
    while (i >= 0)
    {
        int da = a[i] - '0' - borrow;
        int db = (j >= 0) ? b[j--] - '0' : 0;
        if (da < db)
        {
            da += 10;
            borrow = 1;
        }
        else borrow = 0;
        res = char('0' + (da - db)) + res;
        i--;
    }
    return stripLeadingZeros(res);
}

// ---------- Multiplication ----------
string multiplyStrings(string a, string b)
{
    a = stripLeadingZeros(a);
    b = stripLeadingZeros(b);
    if (a == "0" || b == "0") return "0";
    int n = a.length(), m = b.length();
    int res[6005] = {0};
    for (int i = n - 1; i >= 0; i--)
    {
        for (int j = m - 1; j >= 0; j--)
        {
            int p = (a[i] - '0') * (b[j] - '0');
            int sum = p + res[i + j + 1];
            res[i + j + 1] = sum % 10;
            res[i + j] += sum / 10;
        }
    }
    string s = "";
    int i = 0;
    while (i < n + m && res[i] == 0) i++;
    for (; i < n + m; i++) s += char('0' + res[i]);
    return s == "" ? "0" : s;
}

// ---------- Division (quotient only) ----------
string divideStrings(string a, string b)
{
    a = stripLeadingZeros(a);
    b = stripLeadingZeros(b);
    if (b == "0") return "0";
    if (compareStrings(a, b) < 0) return "0";

    string result = "";
    string cur = "";
    for (int i = 0; i < (int)a.length(); i++)
    {
        cur += a[i];
        cur = stripLeadingZeros(cur);
        int q = 0;
        while (compareStrings(cur, b) >= 0)
        {
            cur = subtractStrings(cur, b);
            q++;
        }
        result += char('0' + q);
    }
    return stripLeadingZeros(result);
}

// ---------- Modulo ----------
string modStrings(string a, string b)
{
    a = stripLeadingZeros(a);
    b = stripLeadingZeros(b);
    if (b == "0") return "0";
    if (compareStrings(a, b) < 0) return a;

    string cur = "";
    for (int i = 0; i < (int)a.length(); i++)
    {
        cur += a[i];
        cur = stripLeadingZeros(cur);   
        while (compareStrings(cur, b) >= 0)
        {
            cur = subtractStrings(cur, b);
        }
    }
    return stripLeadingZeros(cur);
}

// ---------- GCD ----------
string gcdStrings(string a, string b)
{
    a = stripLeadingZeros(a);
    b = stripLeadingZeros(b);
    if (a == "0") return b;
    if (b == "0") return a;
    while (b != "0")
    {
        string r = modStrings(a, b);
        a = b;
        b = r;
    }
    return a;
}

// ---------- Increment ----------
string incrementString(string s)
{
    int carry = 1;
    for (int i = s.length() - 1; i >= 0 && carry; i--)
    {
        int d = s[i] - '0' + carry;
        s[i] = char('0' + (d % 10));
        carry = d / 10;
    }
    if (carry) s = '1' + s;
    return stripLeadingZeros(s);
}

// ---------- Factorial (string-based) ----------
string factorialBig(string nStr)
{
    nStr = stripLeadingZeros(nStr);
    if (nStr == "0" || nStr == "1") return "1";

    string i = "2";
    string res = "1";
    while (compareStrings(i, nStr) <= 0)
    {
        res = multiplyStrings(res, i);
        i = incrementString(i);
    }
    return stripLeadingZeros(res);
}

// ---------- Helpers for power ----------
bool isZero(string s)
{
    for (int i = 0; i < (int)s.length(); i++)
        if (s[i] != '0') return false;
    return true;
}

bool isOdd(string s)
{
    int last = s[s.length() - 1] - '0';
    return (last % 2 == 1);
}

string divideBy2(string s)
{
    string res = "";
    int carry = 0;
    for (int i = 0; i < (int)s.length(); i++)
    {
        int cur = carry * 10 + (s[i] - '0');
        int q = cur / 2;
        carry = cur % 2;
        res += char('0' + q);
    }
    return stripLeadingZeros(res);
}

// ---------- Power (string-based exponent) ----------
string powerStrings(string base, string expStr)
{
    base = stripLeadingZeros(base);
    expStr = stripLeadingZeros(expStr);
    if (isZero(expStr)) return "1";

    string res = "1";
    while (!isZero(expStr))
    {
        if (isOdd(expStr))
            res = multiplyStrings(res, base);
        expStr = divideBy2(expStr);
        if (!isZero(expStr))
            base = multiplyStrings(base, base);
    }
    return stripLeadingZeros(res);
}

// ---------- Operator Precedence ----------
int precedence(char c)
{
    if (c == 'x' || c == 'X' || c == '*' || c == '/')
        return 2;
    if (c == '+' || c == '-')
        return 1;
    return 0;
}

// ---------- Expression Evaluation ----------
string evaluateExpression(const string &expr)
{
    string num[2000];
    char op[2000];
    int nTop = 0, oTop = 0;
    string cur = "";

    for (int i = 0; i < (int)expr.length(); i++)
    {
        char c = expr[i];
        if (c >= '0' && c <= '9')
            cur += c;
        else if (c == '+' || c == '-' || c == 'x' || c == 'X' || c == '*' || c == '/')
        {
            num[nTop++] = stripLeadingZeros(cur);
            cur = "";
            while (oTop > 0 && precedence(op[oTop - 1]) >= precedence(c))
            {
                string b = num[--nTop];
                string a = num[--nTop];
                char oper = op[--oTop];
                string res = "0";
                if (oper == '+') res = addStrings(a, b);
                else if (oper == '-') res = subtractStrings(a, b);
                else if (oper == 'x' || oper == 'X' || oper == '*') res = multiplyStrings(a, b);
                else if (oper == '/') res = divideStrings(a, b);
                num[nTop++] = res;
            }
            op[oTop++] = c;
        }
    }
    num[nTop++] = stripLeadingZeros(cur);

    while (oTop > 0)
    {
        string b = num[--nTop];
        string a = num[--nTop];
        char oper = op[--oTop];
        string res = "0";
        if (oper == '+') res = addStrings(a, b);
        else if (oper == '-') res = subtractStrings(a, b);
        else if (oper == 'x' || oper == 'X' || oper == '*') res = multiplyStrings(a, b);
        else if (oper == '/') res = divideStrings(a, b);
        num[nTop++] = res;
    }
    return stripLeadingZeros(num[0]);
}

// ---------- Main ----------
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int type;
    cin >> type;

    if (type == 1)
    {
        string expr;
        cin >> expr;
        cout << evaluateExpression(expr) << "\n";
    }
    else if (type == 2)
    {
        string base, exp;
        cin >> base >> exp;
        cout << powerStrings(base, exp) << "\n";
    }
    else if (type == 3)
    {
        string a, b;
        cin >> a >> b;
        cout << gcdStrings(a, b) << "\n";
    }
    else if (type == 4)
    {
        string n;
        cin >> n;
        cout << factorialBig(n) << "\n";
    }
    return 0;
}
