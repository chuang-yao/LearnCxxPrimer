//
//  Chapter 09 - Sequential Containers
//

#include <iostream>

#include <array>        // fixed-size array
#include <deque>        // double-ended queue
#include <forward_list> // singly linked list
#include <list>         // doubly linked list
#include <stack>        // container adaptor - stack
#include <string>       // specialized container for characters
#include <vector>       // flexiable-size array

using namespace std;

template <typename C> void print(const C &data) {
  for (auto itr = begin(data); itr != end(data); ++itr) {
    cout << *itr << " ";
  }
  cout << endl;
}

int main() {
  { cout << "Hello World!" << endl; }

  {
    vector<int> v{1, 2, 3};
    cout << sizeof v << endl;
    cout << v.size() << endl;
    cout << v.max_size() << endl;
    cout << v.empty() << endl;
  }

  {
    list<string>::iterator itr;
    vector<int>::difference_type count;
    vector<int>::size_type idx;
  }

  {
    list<string> a = {"Milton", "Shakespeare", "Austen"};

    auto it1 = a.begin();   // list<string>::iterator
    auto it2 = a.rbegin();  // list<string>::reverse_iterator
    auto it3 = a.cbegin();  // list<string>::const_iterator
    auto it4 = a.crbegin(); // list<string>::const_reverse_iterator

    list<string>::iterator itr5 = a.begin();
    list<string>::const_iterator itr6 = a.begin();

    auto itr7 = a.begin(); // const_iterator only if a is const
    auto itr8 = a.begin(); // const_iterator
  }

  {
    // we can list-initialize a container in the c++11 standard
    list<string> authors = {"Milton", "Shakespeare", "Austen"};
    vector<const char *> articles = {"a", "an", "the"};

    list<string> list2(authors);
    // vector<string> words(articles); // error! element types do not match!
    forward_list<string> words(articles.begin(), articles.end());
  }

  {
    array<int, 42> arr1;
    array<string, 10> arr2;

    array<int, 10>::size_type i;

    array<int, 10> ia1; // ten default-initialized ints
    array<int, 10> ia2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // list initialization
    array<int, 10> ia3 = {
        42}; // ia3[0] is 42, the rest are value-initialized to zeros

    array<int, 10> digits = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    array<int, 10> copy =
        digits; // this is okay; unsupported for built-in arrays
  }

  {
    vector<int> v1 = {1, 2, 3};
    vector<int> v2{1, 2, 3};
    vector<int> v3(v2);
    vector<int> v4 = v3;
    vector<int> v5(v3.begin(), v3.end());
    vector<int> v6(3, 1);
  }

  {
    list<string> names;
    vector<const char *> oldstyle;
    names.assign(oldstyle.cbegin(), oldstyle.cend());

    list<string> slist1(1);     // one element, which is an empty string
    slist1.assign(10, "Hiya!"); // ten elements; each one is Hiya!
  }

  {
    vector<string> svec1(10);
    vector<string> svec2(24);
    swap(svec1, svec2); // now svec1 has 24 elements and svec2 has 10

    string s1 = "Hello";
    string s2 = "World";
    swap(s1, s2);
    cout << s1 << " " << s2 << endl;
  }

  {
    vector<int> v1 = {1, 3, 5, 7, 9, 12};
    vector<int> v2 = {1, 3, 9};
    vector<int> v3 = {1, 3, 5, 7};
    vector<int> v4 = {1, 3, 5, 7, 9, 12};

    cout << (v1 < v2) << " ";   // True
    cout << (v1 < v3) << " ";   // False
    cout << (v1 == v4) << " ";  // True
    cout << (v1 == v2) << endl; // False
  }

  {
    list<int> ilist;
    for (size_t ix = 0; ix != 4; ++ix) {
      ilist.push_front(ix);
    }
    print(ilist);
  }

  {
    vector<string> svec;
    list<string> slist;

    slist.insert(slist.begin(), "Hello!"); // equivalent to .push_front
    svec.insert(svec.begin(), "Hello!");   // might be slow

    svec.insert(svec.end(), 10, "Anna");

    vector<string> v = {"quasi", "simba", "frollo", "scar"};

    slist.insert(slist.begin(), v.end() - 2, v.end());
    slist.insert(slist.end(),
                 {"these", "words", "will", "go", "at", "the", "end"});

    slist.insert(slist.begin(), slist.begin(), slist.end());

    print(slist);
  }

  {
    vector<string> svec = {"Hello", "World", "!"};
    list<string> slist;
    auto itr = slist.begin();
    for (auto &&e : svec) {
      itr = slist.insert(itr, e); // .insert returns an iterator to the first
                                  // inserted element in c++11
    }
    print(slist);
  }

  {
    vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto e1 = v.back(); // e1 is not a reference! it is a copy of v.back()
    e1 = -1;
    print(v);

    auto &e2 = v.back(); // e2 is a reference to the last element of v
    e2 = -1;
    print(v);
  }

  {
    list<int> lst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto it = lst.begin();
    while (it != lst.end()) {
      if (*it % 2) // check whether the element is odd
      {
        it = lst.erase(it); // .erase returns an iterator to the location after
                            // the deleted element
      } else {
        ++it;
      }
    }
    print(lst);
  }

  {
    forward_list<int> flst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto prev = flst.before_begin(); // off-the-start element
    auto curr = flst.begin();        // first element
    while (curr != flst.end()) {
      if (*curr % 2 == 0) {
        curr = flst.erase_after(prev);
      } else {
        prev = curr;
        ++curr;
      }
    }
    print(flst);
  }

  {
    list<int> ilist(10, 42);
    print(ilist);
    ilist.resize(15);
    print(ilist);
    ilist.resize(25, -1);
    print(ilist);
    ilist.resize(5);
    print(ilist);
  }

  {
    vector<int> vi = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto itr = vi.begin();
    while (itr != vi.end()) {
      if (*itr % 2) {
        itr = vi.insert(itr, *itr);
        itr += 2; // advance to the element past the inserted
      } else {
        itr = vi.erase(itr); // itr is automatically advanced by .erase
      }
    }
    print(vi);
  }

  {
    vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    cout << v.capacity() << " "; // v has a capacity of 10
    v.push_back(10);
    cout << v.capacity() << " "; // v has a capacity of 20
    v.shrink_to_fit(); // the implementation is free to ignore this request
    cout << v.capacity() << endl; // v has a capacity of 11
  }

  {
    vector<int> ivec;
    cout << "ivec: size: " << ivec.size() << " capacity: " << ivec.capacity()
         << endl;

    for (vector<int>::size_type ix = 0; ix != 24; ++ix) {
      ivec.push_back(ix);
    }
    cout << "ivec: size: " << ivec.size() << " capacity: " << ivec.capacity()
         << endl; // capacity allocated varies by implementation

    ivec.reserve(50);
    cout << "ivec: size: " << ivec.size() << " capacity: " << ivec.capacity()
         << endl;

    while (ivec.size() != ivec.capacity()) {
      ivec.push_back(0);
    }
    cout << "ivec: size: " << ivec.size() << " capacity: " << ivec.capacity()
         << endl;

    ivec.push_back(42);
    cout << "ivec: size: " << ivec.size() << " capacity: " << ivec.capacity()
         << endl;

    ivec.shrink_to_fit();
    cout << "ivec: size: " << ivec.size() << " capacity: " << ivec.capacity()
         << endl;
  }

  {
    const char *cp = "Hello World!!!"; // null-terminated C-style string
    char noNull[] = {'H', 'i'};        // not null-terminated

    string s1(cp);        // Hello World!!!
    string s2(noNull, 2); // Hi
    // string s3(noNull); // undefined
    string s4(cp + 6, 5); // World
    string s5(s1, 6, 5);  // World
    string s6(s1, 6);     // World!!!
    string s7(s1, 6, 20); // ok, copies only to end; World!!!
    // string s8(s1, 16); // terminating with uncaught exception of type
    // std::out_of_range: basic_string
  }

  {
    string s("hello world");

    string s2 = s.substr(0, 5);  // hello
    string s3 = s.substr(6);     // world
    string s4 = s.substr(6, 11); // world
    // string s5 = s.substr(12); // terminating with uncaught exception of type
    // std::out_of_range: basic_string
  }

  {
    const char *cp = "Stately, plump Buck";
    string s(cp, 7);            // Stately
    s.insert(s.size(), cp + 7); // Stately, plump Buck
  }

  {
    string s("C++ Primer"), s2 = s;

    s.insert(s.size(), " 4th Ed.");
    s2.append(" 4th Ed.");

    s.erase(11, 3);
    s.insert(11, "5th");
    s2.replace(11, 3, "Fifth");

    cout << s2 << endl;
  }

  {
    string name("AnnaBelle");
    auto pos1 = name.find("Anna");
    cout << pos1 << " ";

    string lowercase("annabelle");
    pos1 = lowercase.find("Anna");
    cout << pos1 << " ";

    string numbers("0123456789"), name2("r2d2");
    auto pos = name2.find_first_of(numbers);
    cout << pos << " ";

    string dept("03714p3");
    pos = dept.find_first_not_of(numbers);
    cout << pos << endl;
  }

  {
    int i = 42;
    string s = to_string(i); // ??? to string
    double d = stod(s);      // string to double
  }

  {
    stack<int> intStack;

    for (size_t ix = 0; ix != 10; ++ix) {
      intStack.push(ix);
    }

    while (!intStack.empty()) {
      int value = intStack.top();
      intStack.pop();
    }
  }
}