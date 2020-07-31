//
//  Chapter 11 - Associative Containers
//

#include <fstream>
#include <iostream>
#include <sstream>

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <utility>

using namespace std;

pair<string, int> process(vector<string> &v) {
  if (!v.empty()) {
    return {v.back(), v.back().size()}; // list-initialized return value
    // return pair<string, int>(v.back(), v.back().size());
    // return make_pair(v.back(), v.back().size());
  } else {
    return pair<string,
                int>(); // value-initialized return value from constructor
  }
}

using namespace std;

map<string, string> buildMap(ifstream &map_file) {
  map<string, string> trans_map;
  string key;
  string value;

  while (map_file >> key && getline(map_file, value)) {
    if (value.size() > 1) {
      trans_map[key] = value.substr(1);
    } else {
      throw runtime_error("no rule for " + key);
    }
  }

  return trans_map;
}

const string &transform(const string &s, const map<string, string> &m) {
  auto map_itr = m.find(s);
  if (map_itr != m.cend()) {
    return map_itr->second;
  } else {
    return s;
  }
}

void word_transform(ifstream &map_file, ifstream &input) {
  auto trans_map = buildMap(map_file);
  string text;
  while (getline(input, text)) {
    istringstream stream(text);
    string word;
    bool firstword = true; // space before word except the first
    while (stream >> word) {
      if (firstword) {
        firstword = false;
      } else {
        cout << " ";
      }
      cout << transform(word, trans_map);
    }
    cout << endl;
  }
}

int main() {
  { cout << "Hello World!" << endl; }
  /*
  {
      map<string, size_t> word_count;
      string word;
      while (cin >> word)
      {
          ++word_count[word];
      }
      for (const auto &w : word_count)
      {
          cout << w.first << " " << w.second << endl;
      }
  }

  {
      map<string, size_t> word_count;
      set<string> exclude = {
          "The", "But", "And", "Or", "An", "A",
          "the", "but", "and", "or", "an", "a"};

      string word;
      while (cin >> word)
      {
          if (exclude.find(word) == exclude.end())
          {
              ++word_count[word];
          }
      }
  }
*/
  {
    set<string> exclude = {"The", "But", "And", "Or", "An", "A",
                           "the", "but", "and", "or", "an", "a"};

    map<string, string> authors = {
        {"Joyce", "James"}, {"Austen", "Jane"}, {"Dickens", "Charles"}};
  }

  {
    vector<int> ivec;
    for (vector<int>::size_type i = 0; i != 10; ++i) {
      ivec.push_back(i);
      ivec.push_back(i);
    }

    set<int> iset(ivec.cbegin(), ivec.cend());
    multiset<int> miset(ivec.cbegin(), ivec.cend());

    cout << ivec.size() << " ";
    cout << iset.size() << " ";
    cout << miset.size() << endl;
  }

  {
    set<string>::value_type v1; // string
    set<string>::key_type v2;   // string

    map<string, int>::value_type v3;  // pari<string, int>
    map<string, int>::key_type v4;    // string
    map<string, int>::mapped_type v5; // int
  }

  {
    map<string, string> authors = {
        {"Joyce", "James"}, {"Austen", "Jane"}, {"Dickens", "Charles"}};

    auto map_itr = authors.begin();
    cout << map_itr->first << " " << map_itr->second << endl;
  }

  {
    set<int> iset = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    set<int>::iterator set_itr = iset.begin();
    while (set_itr != iset.end()) {
      // *set_itr = 42; // Error: keys in a set is read-only
      cout << *set_itr++ << " ";
    }
    cout << endl;
  }

  {
    vector<int> ivec = {2, 4, 6, 8, 2, 4, 6, 8};
    set<int> set2;
    cout << set2.size() << " ";
    set2.insert(ivec.cbegin(), ivec.cend());
    cout << set2.size() << " ";
    set2.insert({1, 3, 5, 7, 1, 3, 5, 7});
    cout << set2.size() << endl;
  }
  /*
  {
      map<string, size_t> word_count;
      string word;

      while (cin >> word)
      {
          auto ret = word_count.insert({word, 1});
          if (!ret.second)
          {
              ++ret.first->second; // equivalent to `++((ret.first)->second);`
          }
      }
  }
*/
  {
    multimap<string, string> authors;
    authors.insert({"Barth, John", "Sot-Weed Factor"});
    authors.insert({"Barth, John", "Lost in the Funhouse"});
  }

  {
    multimap<string, string> authors = {{"Joyce, James", "Ulysses"},
                                        {"Austen, Jane", "Pride and Prejudice"},
                                        {"Dickens, Charles", "Oliver Twist"}};
    authors.insert({"Barth, John", "Sot-Weed Factor"});
    authors.insert({"Barth, John", "Lost in the Funhouse"});

    string search_item("Barth, John");

    for (auto beg = authors.lower_bound(search_item),
              end = authors.upper_bound(search_item);
         beg != end; ++beg) {
      cout << beg->second << endl;
    }

    for (auto pos = authors.equal_range(search_item); pos.first != pos.second;
         ++pos.first) {
      cout << pos.first->second << endl;
    }
  }

  {
    ifstream map("../doc/dict.txt");
    ifstream input("../doc/message.txt");
    word_transform(map, input);
    map.close();
    input.close();
  }
}