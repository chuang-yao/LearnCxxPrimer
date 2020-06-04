#include <iostream>
#include <string>
#include <vector>


using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;


int main() {

    {
        string s1;  // default init, empty string
        string s2 = s1;
        string s3 = "hiya";
        string s4(10, 'x');

        string s5 = "hiya";  // copy init
        string s6("hiya");  // direct init

        string s7(10, 'x');  // direct init
        string s8 = string(10, 'x');  // copy init
    }
    /*
    {
        string line;
        // read input a line at a time untile EOF (control + D)
        while (getline(cin, line)) {
            cout << line << endl;
        }
    }
    */
    {
        string line;
        auto len = line.size();  // len has type string::size_type 
        // WARNING: string::size_type is unsigned)
    }

    {
        string str("some string");
        for (auto c : str) {
            cout << c << " ";
        }
        cout << endl;
    }

    {
        string s("Hello World!!!");
        decltype(s.size()) punct_cnt = 0;
        for (auto c : s) {
            if (ispunct(c)) {
                ++punct_cnt;
            }
        }
        cout << punct_cnt << " punctuation characters in \"" << s << "\"" << endl;
    }

    {
        string s("Hello World!!!");
        for (auto &c : s) {
            c = toupper(c);
        }
        cout << s << endl;
    }

    {
        const string s = "Keep out!";
        for (auto &c : s) {
            cout << c;
        }
        cout << endl;
    }

    {
        vector<vector<int>> ivec;
        //vector<string> svec = ivec;  // no suitable conversion exists
        vector<string> svec(10, "null");
    }

    {
        vector<int> v2;
        for (int i = 0; i != 100; ++i) {
            v2.push_back(i);
        }
    }

    {
        vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
        for (auto &i : v) {
            i *= i;
        }
        for (auto i : v) {
            cout << i << " ";
        }
        cout << endl;
    }

    {
        string s("some string");
        if (s.begin() != s.end()) {
            auto it = s.begin();
            *it = toupper(*it);
        }
        for (auto it = s.begin(); it != s.end() && !isspace(*it); ++it) {
            *it = toupper(*it);
        }
    }

    {
        vector<const int> v{1, 2, 3};
        auto itr_1 = v.begin();  // itr_1 is a std::vector<const int>::iterator

        const vector<int> u{1, 2, 3};
        auto itr_2 = u.begin();  // itr_2 is a std::vector<int>::const_iterator
    }

    {
        vector<int> v{1, 2, 3, 4, 5};
        auto x = v.cend() - v.cbegin(); // x is a std::ptrdiff_t, a signed integral machine-specific type
        auto y = v.cbegin() - v.cend();
        cout << x << endl;  // yields 5
        cout << y << endl;  // yields -5
    }

    {
        int arr[10];
        int *ptr[10];
        int (*Parray)[10] = &arr;
        int (&arrRef)[10] = arr;
    }

    {
        string sa2[10];
        string ia2[10];
        cout << sa2 << " " << ia2 << endl;  // default init inside a function leads to undefined values
    }

    {
        int *p = nullptr;
        int *q = nullptr;
        cout << p - q << endl;
    }
    
}
