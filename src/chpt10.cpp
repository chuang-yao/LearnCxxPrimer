//
//  Chapter 10 - Algorithms
//

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <vector>

using namespace std;
using namespace std::placeholders;

template <typename C> void print(const C &seqc) {
  for (auto itr = cbegin(seqc); itr != cend(seqc); ++itr) {
    cout << *itr << " ";
  }
  cout << endl;
}

void elimDups(vector<string> &words) {
  sort(words.begin(), words.end());
  auto end_unique = unique(words.begin(), words.end());
  words.erase(end_unique, words.end());
}

bool isShorter(const string &s1, const string &s2) {
  return s1.size() < s2.size();
}

bool longerThan5(const string &s) { return s.size() >= 5; }

string make_plural(size_t ctr, const string &word, const string &ending) {
  return ctr > 1 ? word + ending : word;
}

// - print strings whose size is larger than or equal to a certain number
// - use lambda expression to pass more than 2 variables to a predicate
// - specify the seperator used in std::cout with c
void biggies(vector<string> &words, vector<string>::size_type sz,
             ostream &os = cout, char c = ' ') {
  elimDups(words);
  stable_sort(
      words.begin(), words.end(),
      [](const string &s1, const string &s2) { return s1.size() < s2.size(); });
  auto wc = find_if(words.begin(), words.end(),
                    [sz](const string &s) { return s.size() >= sz; });
  auto count = words.end() - wc;
  cout << count << " " << make_plural(count, "word", "s") << " of length " << sz
       << " or longer" << endl;
  for_each(words.begin(), words.end(),
           [&os, c](const string &s) { os << s << c; });
  cout << endl;
}

// capture by value
void fcn1() {
  size_t v1 = 42;
  auto f = [v1] { return v1; };
  v1 = 0;
  auto j = f(); // j is 42; f captured a copy of v1
}

// capture by reference
void fcn2() {
  size_t v1 = 42;
  auto f2 = [&v1] { return v1; };
  v1 = 0;
  auto j = f2(); // j is 0; f2 captured a reference of v1
}

// mutable lambda
void fcn3() {
  size_t v1 = 42;
  auto f = [v1]() mutable { return ++v1; };
  v1 = 0;
  auto j = f(); // j is 43
}

void fcn4() {
  size_t v1 = 42;
  auto f2 = [&v1]() { return ++v1; }; // v1 refers to a non-const variable
  v1 = 0;
  auto j = f2(); // j is 1
}

bool check_size(const string &s, string::size_type sz) {
  return s.size() >= sz;
}

int main() {
  { cout << "Hello World!" << endl; }

  {
    vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int val = 42;
    auto result = find(v.begin(), v.end(), val);
    cout << "The value " << val
         << (result == v.end() ? " is not present" : " is present") << endl;
  }

  {
    int ia[] = {27, 210, 12, 47, 109, 83};
    int val = 109;
    int *result = find(begin(ia), end(ia), val);
    cout << *(result - 1) << endl;
  }

  {
    vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int sum = accumulate(v.begin(), v.end(),
                         0); // return type depends on the third argument
    cout << sum << endl;
  }

  {
    vector<string> roster1{"Hello", "World"};
    list<const char *> roster2{"Hello", "World", "!!!"};
    auto result = equal(roster1.cbegin(), roster1.cend(), roster2.cbegin());
    cout << result << endl; // true
  }

  {
    vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    print(v);
    fill(v.begin(), v.end(), 0);
    print(v);
  }

  {
    vector<int> vec;
    auto itr = back_inserter(vec);
    *itr = 42;
    print(vec);
    fill_n(back_inserter(vec), 10, 0);
    print(vec);
  }

  {
    int a1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int a2[sizeof(a1) / sizeof(*a1)];
    auto ret = copy(begin(a1), end(a1),
                    a2); // ret will point just past the last element copied
    print(a2);
  }

  {
    vector<string> words = {"the",  "quick", "red",  "fox", "jumps",
                            "over", "the",   "slow", "red", "turtle"};

    // isShorter meets the requirements of being a predicate
    auto words1 = words;
    elimDups(words1);
    sort(words1.begin(), words1.end(), isShorter);
    print(words1);

    // stable sort maintains the original order among equal elements
    auto words2 = words;
    elimDups(words2);
    stable_sort(words2.begin(), words2.end(), isShorter);
    print(words2);

    auto words3 = words;
    auto pos3 = partition(words3.begin(), words3.end(), longerThan5);
    print(words3);
    cout << *pos3 << endl; // fox
  }

  // lambda expression
  // [capture list] (parameter list) -> return type { function body }

  {
    auto f = [] { return 42; };
    cout << f() << endl;
  }

  {
    vector<string> words = {"the",  "quick", "red",  "fox", "jumps",
                            "over", "the",   "slow", "red", "turtle"};
    biggies(words, 5);
  }

  {
    vector<int> ivec = {0, -1, 2, -3, 4, -5, 6, -7, 8, -9};
    transform(ivec.begin(), ivec.end(), ivec.begin(), [](int i) -> int {
      if (i < 0)
        return -i;
      else
        return i;
    });
    print(ivec);
  }

  {
    auto check6 = bind(check_size, _1, 6);
    string s = "hello";
    bool b1 = check6(s); // false
  }

  {
    list<int> lst = {1, 2, 3, 4};
    list<int> lst2, lst3;
    copy(lst.cbegin(), lst.cend(), front_inserter(lst2));
    print(lst2); // 4 3 2 1
    copy(lst.cbegin(), lst.cend(), inserter(lst3, lst3.begin()));
    print(lst3); // 1 2 3 4
  }
  /*
      {
          istream_iterator<int> int_iter(cin), eof;
          vector<int> vec(int_iter, eof); // this constructor reads cin until it
     hits end-of-file print(vec);
      }

      {
          istream_iterator<int> in(cin), eof;
          cout << accumulate(in, eof, 0) << endl;
      }
   */
  {
    vector<int> vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    ostream_iterator<int> out_iter(cout, " ");
    for (const auto &e : vec) {
      out_iter = e; // equivalent to `*out_iter++ = e;`
    }
    cout << endl;

    copy(vec.begin(), vec.end(), out_iter);
    cout << endl;
  }
}