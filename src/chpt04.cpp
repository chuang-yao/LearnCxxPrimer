#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

template <typename T1, typename T2>
double l2Norm(vector<T1> x, vector<T2> y)
{
    auto itr_x = x.cbegin();
    auto itr_y = y.cbegin();
    double s = 0.0;

    while (itr_x != x.cend() || itr_y != y.cend())
    {
        if (itr_x == x.cend() && itr_y != y.cend())
        {
            s += (*itr_y) * (*itr_y);
            ++itr_y;
        }
        else if (itr_x != x.cend() && itr_y == y.cend())
        {
            s += (*itr_x) * (*itr_x);
            ++itr_x;
        }
        else
        {
            s += (*itr_x - *itr_y) * (*itr_x - *itr_y);
            ++itr_x;
            ++itr_y;
        }
    }

    return std::sqrt(s);
}

int main()
{

    {
        int x = 1;
        int *p = &x;
        decltype(*p) a = x;  // a is int&
        decltype(&p) b = &p; // b is int**
    }

    {
        string text = "Hello World!";
        for (const auto &c : text)
        {
            cout << c << " ";
        }
        cout << endl;
    }

    {
        const char *cp = "Hello World!";
        if (cp && *cp)
        {
            cout << "true" << endl;
        }
        else
        {
            cout << "false" << endl;
        }
    }

    {
        int i = 2, j = 3;
        i = j = 1; // the assignment operator is right-associative
        cout << i << " " << j << endl;
    }

    {
        int i = 0;
        double d = 0.0;
        // d = i = 3.5;
        cout << d << " " << i << endl; // WARNING: implicit conversion from 'double' to 'int'
    }

    {
        double dval;
        int ival;
        int *pi = &ival; // pi must be initialized before modifying the object it points to
        dval = ival = *pi = 0;
    }

    {
        vector<int> v1{0, 0};
        vector<double> v2{0.0, 1.0, 2.0};
        cout << l2Norm(v1, v2) << endl;
    }

    {
        int i = 0, j = 0;
        j = ++i;
        cout << i << " " << j << endl; // (i, j) is (1, 1)
        j = i++;
        cout << i << " " << j << endl; // (i, j) is (2, 1)
    }

    {
        vector<int> v{3, 2, 1, 0, -1, -2, -3};
        auto pbeg = v.begin();
        while (pbeg != v.end() && *pbeg >= 0)
        {
            cout << *pbeg++ << endl; // a common idiom in C++
        }
    }

    {
        string s = "hello world!";
        auto beg = s.begin();
        while (beg != s.end() && !isspace(*beg))
        {
            // *beg = toupper(*beg++);  // undefined behavior!
            *beg = toupper(*beg);
            ++beg;
        }
        cout << s << endl;
    }

    {
        string s1 = "a string", *p = &s1;
        auto n = s1.size();
        n = (*p).size();
        n = p->size();
    }

    {
        vector<string> vs{"Hello", "World"};
        vector<string>::iterator iter = vs.begin();
        cout << *++iter << endl;
        cout << iter++->empty() << endl;
    }

    {
        int grade = 80;
        string letterGrade = (grade < 60) ? "Fail" : "Pass";
        cout << letterGrade << endl;
    }

    {
        unsigned long quiz1 = 0;  // use the value as a collection of bits

        quiz1 |= 1UL << 27;
        quiz1 &= ~(1UL << 27);

        bool status = quiz1 & (1UL << 27);
        cout << status << endl;
    }

    {
        int* p;
        cout << sizeof p << endl;
        cout << sizeof *p << endl;  // sizeof does not evaluate its operand
    }

    {
        int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        constexpr size_t sz = sizeof(a) / sizeof(a[0]);
        int b[sz] = {};
    }

    {
        vector<int> v{0, 1, 2, 3, 4, 5};
        vector<int>::size_type cnt = v.size();
        for (vector<int>::size_type i = 0; i != v.size(); ++i, --cnt) {
            v[i] = cnt;
        }
        for (const auto &e : v) {
            cout << e << " ";
        }
        cout << endl;
    }

    {
        constexpr int size = 5;
        int ia[size] = {1, 2, 3, 4, 5};
        for (int *ptr = ia, ix = 0; ix != size && ptr != ia + size; ++ix, ++ptr) {
            cout << *ptr << " ";
        }
        cout << endl;
    }

    {
        double d = 3.14159;
        void *p = &d;
        double *dp = static_cast<double*>(p);
        cout << *dp << endl;
    }

    {
        const char *pc;
        char *p = const_cast<char*>(pc);  // okay, but writing through p is undefined
    }

    {
        const char *cp;
        //char *q = static_cast<char*>(cp);  // error: static_cast cannot cast aways const
        static_cast<string>(cp);
        //const_cast<string>(cp);  // the type in a const_cast must be a pointer, reference, or pointer to member to an object type
        // const_cast only changes constness
    }

    {
        int *ip;
        char *pc = reinterpret_cast<char*>(ip);  // almost always hazardous
        string str(pc);  // likely to fail at runtime
    }
}
