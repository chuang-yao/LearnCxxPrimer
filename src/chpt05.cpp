#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using std::cin;
using std::cout;
using std::endl;
using std::runtime_error;
using std::string;
using std::vector;

template <typename T>
void print(vector<T> v)
{
    for (auto &&e : v)
    {
        cout << e << " ";
    }
    cout << endl;
}

int main()
{

    {
        int ival = 0;
        // ival + 5;  // WARNING: legal but useless
        cout << ival << endl;
    }

    {
        ; // a null statement
    }

    {
        int ival = 0, v1 = 1, v2 = 2;
        ival = v1 + v2;
        ; // second semicolon is a null statement
        cout << ival << endl;
    }

    {
        int i = 10;
        while (--i != 0)
        {
        } // a block can be empty
        cout << i << endl;
    }

    {
        const vector<string> scores = {"F", "D", "C", "B", "A", "A++"};
        string letterGrade;
        int grade = 100;
        if (grade < 60)
        {
            letterGrade = scores[0];
        }
        else
        {
            letterGrade = scores[(grade - 50) / 10];
        }
        cout << grade << " : " << letterGrade << endl;
    }

    {
        vector<int> v = {1, 2, 3};
        for (decltype(v.size()) i = 0, sz = v.size(); i != sz; ++i)
        {
            v.push_back(v[i]);
        }
        print(v);
    }

    {
        vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        for (auto &r : v)
        {
            r *= r;
        }
        print(v);
    }

    {
        try
        {
            throw runtime_error("An Error Has Occurred! (not really)");
        }
        catch (runtime_error err)
        {
            cout << err.what() << endl;
            return -1;
        }
    }
}