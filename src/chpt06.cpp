#include <cassert>
#include <iostream>

#include "chpt06.hpp"

using std::begin;
using std::cin;
using std::cout;
using std::end;
using std::endl;
using std::initializer_list;
using std::string;
using std::vector;

int main()
{
    {
        cout << "Hello World!" << endl;
    }

    {
        int i = fact(5);
        cout << i << endl;
    }

    {
        cout << myFact(6) << endl;
    }

    {
        for (size_t i = 0; i != 10; ++i)
        {
            cout << countCalls() << " ";
        }
        cout << endl;
    }

    {
        int i = 1;
        resetInt(i);
        cout << i << endl;
    }

    {
        const int ci = 42; // top-level const
        int i = ci;        // copy is allowed
        int *const p = &i; // top-level const
        *p = 43;           // change through p is allowed
        cout << i << endl;
    }

    {
        int i = 1;
        const int ci = i;
        cout << i << endl;

        resetInt(i);
        resetInt(&i);

        cout << i << endl;
    }

    {
        int *p[10];  // an array of 10 int pointers
        int(*q)[10]; // a pointer to an array of 10 ints
    }

    {
        error_msg({"Error_1", "Error_2", "Error_3"});
    }

    {
        cout << listSum({1, 2, 3, 4, 5}) << endl;
    }

    {
        int a = 1, b = 2;
        swapInt(a, b);
        cout << a << " " << b << endl;
    }

    {
        string s = "Hello World?";
        cout << s << endl;
        get_val(s, 11) = '!';
        cout << s << endl;
    }

    {
        typedef int arr[10];  // arrT is of type 'array of 10 ints'
        using arrT = int[10]; // equivalent to above
    }

    {
        print("Printing a C-Style String...");

        constexpr size_t sz = 5;
        int a[5] = {0, 1, 2, 3, 4};

        print(a, a + sz);
        print(a, end(a) - begin(a));
        print(a, sz);
    }

    {
        const string s1 = "Name", s2 = "Birthday";
        string s3 = "Alice", s4 = "Bob";

        cout << shorterString(s1, s2) << endl;
        cout << shorterString(s3, s4) << endl;

        shorterString(s3, s4) = "Calvin";
        cout << s4 << endl;
    }

    {
#ifndef NDEBUG
        cout << "Program is running in debug mode..." << endl;
#endif
        assert(1 > 2); // stops the program if NDEBUG is not defined
    }
}

template <typename T>
void print(vector<T> v)
{
    for (auto &&e : v)
    {
        cout << e << " ";
    }
    cout << endl;
}

void print(const char *cp)
{
    while (*cp != '\0')
    {
        cout << *cp++;
    }
    cout << endl;
}

void print(const int *beg, const int *end)
{
    for (auto itr = beg; itr != end; ++itr)
    {
        cout << *itr << " ";
    }
    cout << endl;
}

void print(const int ai[], size_t sz)
{
    for (size_t ix = 0; ix != sz; ++ix)
    {
        cout << ai[ix] << " ";
    }
    cout << endl;
}

int fact(int val)
{
    int ret = 1;
    while (val > 1)
    {
        ret *= val--;
    }
    return ret;
}

int myFact(int i)
{
    if (i == 0)
    {
        return 1;
    }
    else if (i < 0)
    {
        return 0;
    }
    else
    {
        return i * myFact(i - 1);
    }
}

// local static objects are persistent through the entire program
size_t countCalls()
{
    static size_t ctr = 0;
    return ++ctr;
}

void resetInt(int &i)
{
    i = 0;
    cout << "resetInt: pass by reference" << endl;
}

void resetInt(int *p)
{
    *p = 0;
    cout << "resetInt: pass by pointer" << endl;
}

void error_msg(initializer_list<string> il)
{
    for (auto itr = il.begin(); itr != il.end(); ++itr)
    {
        cout << *itr << " ";
    }
    cout << endl;
}

int listSum(initializer_list<int> il)
{
    int sum = 0;
    for (auto itr = il.begin(); itr != il.end(); ++itr)
    {
        sum += *itr;
    }
    return sum;
}

void swapInt(int &v1, int &v2)
{
    if (v1 == v2)
    {
        return;
    }
    int tmp = v1;
    v1 = v2;
    v2 = tmp;
}

char &get_val(string &str, string::size_type ix)
{
    return str[ix];
}
