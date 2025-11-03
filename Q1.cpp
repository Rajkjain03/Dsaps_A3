#include <iostream>
#include <string>
using namespace std;

// ---------- Utility ----------
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
        if (A[i] < B[i])
            return -1;
        if (A[i] > B[i])
            return 1;
    }
    return 0;
}

string reverseStr(const string &s)
{
    string t = s;
    int n = s.length();
    for (int i = 0; i < n / 2; i++)
    {
        char tmp = t[i];
        t[i] = t[n - 1 - i];
        t[n - 1 - i] = tmp;
    }
    return t;
}

// ---------- Addition ----------
string addStrings(string a, string b)
{
    a = reverseStr(a);
    b = reverseStr(b);
    string res = "";
    int carry = 0;
    int n = (a.length() > b.length() ? a.length() : b.length());
    for (int i = 0; i < n; i++)
    {
        int da = (i < (int)a.length()) ? (a[i] - '0') : 0;
        int db = (i < (int)b.length()) ? (b[i] - '0') : 0;
        int sum = da + db + carry;
        res += char('0' + (sum % 10));
        carry = sum / 10;
    }
    if (carry)
        res += char('0' + carry);
    return stripLeadingZeros(reverseStr(res));
}

// ---------- Subtraction (a >= b) ----------
string subtractStrings(string a, string b)
{
    a = reverseStr(a);
    b = reverseStr(b);
    string res = "";
    int borrow = 0;
    for (int i = 0; i < (int)a.length(); i++)
    {
        int da = a[i] - '0';
        int db = (i < (int)b.length()) ? (b[i] - '0') : 0;
        da -= borrow;
        if (da < db)
        {
            da += 10;
            borrow = 1;
        }
        else
            borrow = 0;
        res += char('0' + (da - db));
    }
    res = reverseStr(res);
    return stripLeadingZeros(res);
}

// ---------- Multiplication ----------
string multiplyStrings(string a, string b)
{
    a = stripLeadingZeros(a);
    b = stripLeadingZeros(b);
    if (a == "0" || b == "0")
        return "0";
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
    while (i < n + m && res[i] == 0)
        i++;
    for (; i < n + m; i++)
        s += char('0' + res[i]);
    return s == "" ? "0" : s;
}

// ---------- Division ----------
pair<string, string> divideStrings(string a, string b)
{
    a = stripLeadingZeros(a);
    b = stripLeadingZeros(b);
    if (b == "0")
        return make_pair("0", "0");
    if (compareStrings(a, b) < 0)
        return make_pair("0", a);

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
    return make_pair(stripLeadingZeros(result), stripLeadingZeros(cur));
}

// ---------- GCD ----------
string gcdStrings(string a, string b)
{
    a = stripLeadingZeros(a);
    b = stripLeadingZeros(b);
    if (a == "0")
        return b;
    if (b == "0")
        return a;
    while (b != "0")
    {
        string r = divideStrings(a, b).second;
        a = b;
        b = r;
    }
    return a;
}

// ---------- Power ----------
string powerStrings(string base, string expStr)
{
    unsigned long long exp = 0;
    for (int i = 0; i < (int)expStr.length(); i++)
        exp = exp * 10 + (expStr[i] - '0');
    string res = "1";
    while (exp > 0)
    {
        if (exp & 1ULL)
            res = multiplyStrings(res, base);
        exp >>= 1;
        if (exp)
            base = multiplyStrings(base, base);
    }
    return stripLeadingZeros(res);
}

// ---------- Factorial ----------
string factorialString(string nStr)
{
    unsigned long long n = 0;
    for (int i = 0; i < (int)nStr.length(); i++)
        n = n * 10 + (nStr[i] - '0');
    string res = "1";
    for (unsigned long long i = 2; i <= n; i++)
    {
        string si = "";
        unsigned long long x = i;
        while (x > 0)
        {
            si = char('0' + (x % 10)) + si;
            x /= 10;
        }
        res = multiplyStrings(res, si);
    }
    return stripLeadingZeros(res);
}

// ---------- Expression Evaluation ----------
int precedence(char c)
{
    if (c == 'x' || c == 'X' || c == '/')
        return 2;
    if (c == '+' || c == '-')
        return 1;
    return 0;
}

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
        else if (c == '+' || c == '-' || c == 'x' || c == 'X' || c == '/')
        {
            num[nTop++] = stripLeadingZeros(cur);
            cur = "";
            while (oTop > 0 && precedence(op[oTop - 1]) >= precedence(c))
            {
                string b = num[--nTop];
                string a = num[--nTop];
                char oper = op[--oTop];
                string res = "0";
                if (oper == '+')
                    res = addStrings(a, b);
                else if (oper == '-')
                    res = subtractStrings(a, b);
                else if (oper == 'x' || oper == 'X')
                    res = multiplyStrings(a, b);
                else if (oper == '/')
                    res = divideStrings(a, b).first;
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
        if (oper == '+')
            res = addStrings(a, b);
        else if (oper == '-')
            res = subtractStrings(a, b);
        else if (oper == 'x' || oper == 'X')
            res = multiplyStrings(a, b);
        else if (oper == '/')
            res = divideStrings(a, b).first;
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
        cout << factorialString(n) << "\n";
    }
    return 0;
}
