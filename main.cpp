#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>


using namespace std;

class big_int {

private:

    explicit big_int(int n);

public:

    vector<int> digit;

    bool negate;

    explicit big_int(string str);

    static const int BASE = 1000000000;

    explicit big_int();

};

void delete_zero(big_int &b) {
    while (b.digit.size() > 1 && b.digit.back() == 0) {
        b.digit.pop_back();
    }
    if (b.digit.size() == 1 && b.digit[0] == 0) b.negate = false;
}

big_int::big_int(string str) {
    if (str.length() == 0) {
        this->negate = false;
    } else {
        if (str[0] == '-') {
            str = str.substr(1);
            this->negate = true;
        } else {
            this->negate = false;
        }
        for (long long i = str.length(); i > 0; i -= 9) {
            if (i < 9)
                this->digit.push_back(atoi(str.substr(0, i).c_str()));
            else
                this->digit.push_back(atoi(str.substr(i - 9, 9).c_str()));
        }
        delete_zero(*this);
    }
}

big_int::big_int(int n) {
    if (n < 0) {
        this->negate = true;
        n = -n;
    } else {
        this->negate = false;
    }
    do {
        this->digit.push_back(n % big_int::BASE);
        n /= big_int::BASE;
    } while (n != 0);
}


big_int::big_int() {
    this->negate = false;
}


bool operator==(const big_int &a, const big_int &b) {
    if (a.negate != b.negate) {
        return false;
    }
    if (a.digit.empty()) {
        return b.digit.empty() || (b.digit.size() == 1 && b.digit[0] == 0);
    }

    if (b.digit.empty()) {
        return a.digit.size() == 1 && a.digit[0] == 0;
    }

    if (a.digit.size() != b.digit.size()) {
        return false;
    }
    for (size_t i = 0; i < a.digit.size(); ++i) {
        if (a.digit[i] != b.digit[i]) {
            return false;
        }
    }

    return true;
}

bool operator!=(const big_int &a, const big_int &b) {

    return !(a == b);
}


big_int opposite(const big_int &b) {
    big_int copy(b);
    copy.negate = !copy.negate;

    return copy;
}

bool operator<(const big_int &a, const big_int &b) {

    if (a == b) {
        return false;
    }

    if (a.negate) {
        if (b.negate) {
            return ((opposite(b)) < (opposite(a)));
        } else {
            return true;
        }
    } else if (b.negate) {
        return false;
    } else {
        if (a.digit.size() != b.digit.size()) {
            return a.digit.size() < b.digit.size();
        } else {
            for (long long i = a.digit.size() - 1; i >= 0; --i) {
                if (a.digit[i] != b.digit[i]) {
                    return a.digit[i] < b.digit[i];
                }
            }

            return false;
        }
    }
}

bool operator>=(const big_int &a, const big_int &b) {

    return !(a < b);
}

bool operator>(const big_int &a, const big_int &b) {

    return !(a < b) && a != b;
}

bool operator<=(const big_int &a, const big_int &b) {

    return !(a > b);
}

string to_str(const big_int &b) {
    string s;
    s = "";
    if (b.negate) {
        s += "-";
    }
    if (b.digit.empty()) {
        s += "0";
    } else {
        s += to_string(b.digit.back());
        for (int i = (int) b.digit.size() - 2; i >= 0; --i) {
            string tmp = to_string(b.digit[i]);
            for (int j = 0; j < 9 - tmp.size(); j++) {
                s += "0";
            }
            s += tmp;
        }
    }

    return s;
}

big_int operator-(big_int a, big_int b);


big_int operator+(big_int a, big_int b) {
    if (a.negate) {
        if (b.negate) return opposite((opposite(a) + (opposite(b))));
        else return b - (opposite(a));
    } else if (b.negate) return a - (opposite(b));
    int c = 0;
    for (size_t i = 0; i < std::max(a.digit.size(), b.digit.size()) || c != 0; ++i) {
        if (i == a.digit.size()) a.digit.push_back(0);
        a.digit[i] += c + (i < b.digit.size() ? b.digit[i] : 0);
        c = a.digit[i] >= big_int::BASE;
        if (c != 0) a.digit[i] -= big_int::BASE;
    }

    delete_zero(a);

    return a;
}

big_int operator-(big_int a, big_int b) {
    if (b.negate) return a + (opposite(b));
    else if (a.negate) return opposite((opposite(a) + b));
    else if (a < b) return opposite(b - a);

    int c = 0;

    for (size_t i = 0; i < b.digit.size() || c != 0; ++i) {
        a.digit[i] -= c + (i < b.digit.size() ? b.digit[i] : 0);
        c = a.digit[i] < 0;
        if (c != 0) a.digit[i] += big_int::BASE;
    }

    delete_zero(a);

    return a;
}

big_int operator*(const big_int &a, const big_int &b) {
    big_int c("");
    c.digit.resize(a.digit.size() + b.digit.size());

    for (size_t i = 0; i < a.digit.size(); ++i)
        for (int j = 0, d = 0; j < (int) b.digit.size() || d; ++j) {
            long long cur =
                    c.digit[i + j] + a.digit[i] * 1ll * (j < (int) b.digit.size() ? b.digit[j] : 0) +
                    d;
            c.digit[i + j] = int(cur % big_int::BASE);
            d = int(cur / big_int::BASE);
        }

    delete_zero(c);

    c.negate = (a.negate ^ b.negate);

    return c;
}

void shift_right(big_int &b) {
    if (b.digit.empty()) {
        b.digit.push_back(0);
        return;
    }
    b.digit.push_back(b.digit[b.digit.size() - 1]);
    for (size_t i = b.digit.size() - 2; i > 0; --i) b.digit[i] = b.digit[i - 1];
    b.digit[0] = 0;
}

big_int operator/(const big_int &x, const big_int &y) {
    big_int b = y;
    b.negate = false;
    big_int c(""), current("");
    c.digit.resize(x.digit.size());
    for (long long i = static_cast<long long>(x.digit.size()) - 1; i >= 0; --i) {
        shift_right(current);
        current.digit[0] = x.digit[i];
        while (current.digit.size() > 1 && current.digit.back() == 0)
            current.digit.pop_back();
        int z = 0, l = 0, r = big_int::BASE;
        while (l <= r) {
            int m = (l + r) / 2;
            big_int t = b * big_int(to_string(m));
            if (t <= current) {
                z = m;
                l = m + 1;
            } else r = m - 1;
        }

        c.digit[i] = z;
        current = current - b * big_int(to_string(z));
    }

    c.negate = (x.negate ^ y.negate);

    delete_zero(c);

    return c;
}

big_int sqrt(big_int &n) {
    big_int a = n + big_int("1");
    vector<big_int> b;
    b.push_back(n);
    while (b.back() < a) {
        a = b.back();
        big_int tmp = (a + n / a) / big_int("2");
        b.pop_back();
        b.push_back(tmp);
    }
    b.clear();
    return a;
}

big_int operator%(const big_int &left, const big_int &right) {
    big_int result = left - (left / right) * right;

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Quantity of arguments is incorrect!\nargument 1: lab4\nargument 2: <name of input file>\nargument 3: <name of output file>");
        return 1;
    }

    if ((argv[1] == nullptr) || (argv[2] == nullptr)) {
        printf("Problem with name of files!");
        return 1;
    }

    ifstream fin;

    fin.open(argv[1]);
    if (!fin.is_open()) {
        printf("Can't open input file");
        return 1;
    }

    string n1, n2;
    int ans = 0;
    string answer;
    char sign[2];
    getline(fin, n1);
    fin >> sign;
    big_int b1(n1);
    if (strcmp(sign, "#") != 0) {
        fin >> n2;
    }
    big_int b2(n2);

    fin.close();

    ofstream fout;
    fout.open(argv[2]);


    if (!fout.is_open()) {
        printf("Can't open output file");
        return 1;
    }

    if (!strcmp(sign, "==")) {
        if (b1 == b2) {
            ans = 1;
        } else {
            ans = 0;
        }
        fout << ans;
    } else if (!strcmp(sign, "!=")) {
        if (b1 != b2) {
            ans = 1;
        } else {
            ans = 0;
        }
        fout << ans;
    } else if (!strcmp(sign, "<")) {
        if (b1 < b2) {
            ans = 1;
        } else {
            ans = 0;
        }
        fout << ans;
    } else if (!strcmp(sign, ">=")) {
        if (b1 >= b2) {
            ans = 1;
        } else {
            ans = 0;
        }
        fout << ans;
    } else if (!strcmp(sign, ">")) {
        if (b1 > b2) {
            ans = 1;
        } else {
            ans = 0;
        }
        fout << ans;
    } else if (!strcmp(sign, "<=")) {
        if (b1 <= b2) {
            ans = 1;
        } else {
            ans = 0;
        }
        fout << ans;
    } else if (!strcmp(sign, "+")) {
        answer = to_str(b1 + b2);
        fout << answer;
    } else if (!strcmp(sign, "-")) {
        answer = to_str(b1 - b2);
        fout << answer;
    } else if (!strcmp(sign, "*")) {
        answer = to_str(b1 * b2);
        fout << answer;
    } else if (!strcmp(sign, "/")) {
        if (!strcmp(to_str(b2).c_str(), "0")) {
            fout << "NaN";
        } else {
            answer = to_str(b1 / b2);
            fout << answer;
        }
    } else if (!strcmp(sign, "%")) {
        if (!strcmp(to_str(b2).c_str(), "0")) {
            fout << "NaN";
        } else {
            answer = to_str(b1 % b2);
            fout << answer;
        }
    } else if (!strcmp(sign, "#")) {
        if (b1.negate) {
            fout << "NaN";
        } else {
            answer = to_str(sqrt(b1));
            fout << answer;
        }
    }

    fout.close();
    return 0;
}
