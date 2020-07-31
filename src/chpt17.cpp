//
//  Chapter 17 - Specialized Language Facilities
//

#include <algorithm>
#include <bitset>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <string>
#include <tuple>
#include <vector>

/* -------------------------------- BookStore ------------------------------- */

class Sales_data {
  friend Sales_data add(const Sales_data &, const Sales_data &);
  friend std::ostream &print(std::ostream &, const Sales_data &);
  friend std::istream &read(std::istream &, Sales_data &);
  friend std::ostream &operator<<(std::ostream &, const Sales_data &);
  friend std::istream &operator>>(std::istream &, Sales_data &);
  friend Sales_data operator+(const Sales_data &, const Sales_data &);
  friend bool operator==(const Sales_data &, const Sales_data &);
  friend bool operator!=(const Sales_data &, const Sales_data &);
  friend class std::hash<Sales_data>;

public:
  Sales_data(const std::string &s, unsigned n, double p)
      : bookNo(s), units_sold(n), revenue(p * n) {}
  Sales_data() : Sales_data("", 0, 0) {}
  Sales_data(const std::string &s) : Sales_data(s, 0, 0) {}
  explicit Sales_data(std::istream &is) : Sales_data() { read(is, *this); }

  std::string isbn() const { return bookNo; } // implicitly inline
  Sales_data &combine(const Sales_data &);

  Sales_data &operator+=(const Sales_data &);

private:
  std::string bookNo;
  unsigned units_sold = 0;
  double revenue = 0.0;

  double avg_price() const { return units_sold ? revenue / units_sold : 0; }
};

Sales_data &Sales_data::combine(const Sales_data &rhs) {
  units_sold += rhs.units_sold;
  revenue += rhs.revenue;
  return *this;
}

Sales_data add(const Sales_data &lhs, const Sales_data &rhs) {
  Sales_data sum = lhs;
  sum.combine(rhs);
  return sum;
}

std::istream &read(std::istream &is, Sales_data &item) {
  double price = 0.0;
  is >> item.bookNo >> item.units_sold >> price;
  item.revenue = price * item.units_sold;
  return is;
}

std::ostream &print(std::ostream &os, const Sales_data &item) {
  os << item.isbn() << " " << item.units_sold << " " << item.revenue << " "
     << item.avg_price();
  return os;
}

std::ostream &operator<<(std::ostream &os, const Sales_data &item) {
  os << item.isbn() << " " << item.units_sold << " " << item.revenue << " "
     << item.avg_price(); // minimal formatting; does not print newline
  return os;
}

std::istream &operator>>(std::istream &is, Sales_data &item) {
  double price;
  is >> item.bookNo >> item.units_sold >> price;
  if (is) { // check if the input was successful
    item.revenue = item.units_sold * price;
  } else {
    item = Sales_data(); // reset the item to default constructed
  }
  return is;
}

Sales_data &Sales_data::operator+=(const Sales_data &rhs) {
  units_sold += rhs.units_sold;
  revenue += rhs.revenue;
  return *this;
}

Sales_data operator+(const Sales_data &lhs, const Sales_data &rhs) {
  Sales_data sum = lhs;
  sum += rhs;
  return sum;
}

bool operator==(const Sales_data &lhs, const Sales_data &rhs) {
  return lhs.isbn() == rhs.isbn() && lhs.units_sold == rhs.units_sold &&
         lhs.revenue == rhs.revenue;
}

bool operator!=(const Sales_data &lhs, const Sales_data &rhs) {
  return !(rhs == lhs);
}

namespace std {

template <> struct hash<Sales_data> {
  typedef size_t result_type;
  typedef Sales_data argument_type;
  size_t operator()(const Sales_data &s) const;
};

size_t hash<Sales_data>::operator()(const Sales_data &s) const {
  return hash<string>()(s.bookNo) ^ hash<unsigned>()(s.units_sold) ^
         hash<double>()(s.revenue);
}
} // namespace std

bool compareIsbn(const Sales_data &lhs, const Sales_data &rhs) {
  return lhs.isbn() < rhs.isbn();
}

typedef std::tuple<std::vector<Sales_data>::size_type,
                   std::vector<Sales_data>::const_iterator,
                   std::vector<Sales_data>::const_iterator>
    matches;

std::vector<matches> findBook(const std::vector<std::vector<Sales_data>> &files,
                              const std::string &book) {
  std::vector<matches> ret;

  for (auto it = files.cbegin(); it != files.cend(); ++it) {
    auto found =
        std::equal_range(it->cbegin(), it->cend(), book,
                         [](const Sales_data &lhs, const Sales_data &rhs) {
                           return lhs.isbn() < rhs.isbn();
                         });
    if (found.first != found.second) { // if the found range is empty
      ret.push_back(
          std::make_tuple(it - files.cbegin(), found.first, found.second));
    }
  }

  return ret;
}

void reportResults(std::istream &in, std::ostream &os,
                   const std::vector<std::vector<Sales_data>> &files) {
  std::string s;
  while (in >> s) {
    auto trans = findBook(files, s);
    if (trans.empty()) {
      std::cout << s << " not found in any stores" << std::endl;
      continue;
    }
    for (const auto &store : trans) {
      os << "store " << std::get<0>(store) << " sales: "
         << std::accumulate(std::get<1>(store), std::get<2>(store),
                            Sales_data(s))
         << std::endl;
    }
  }
}

/* -------------------------------------------------------------------------- */

// this function generates the SAME sequence on each call
std::vector<unsigned> bad_randVec() {
  std::default_random_engine e;
  std::uniform_int_distribution<unsigned> u(0, 9);

  std::vector<unsigned> ret;
  for (size_t i = 0; i != 100; ++i) {
    ret.push_back(u(e));
  }
  return ret;
}

// static objects preserve their states throughout the program
// new numbers generated on each call
std::vector<unsigned> good_randVec() {
  static std::default_random_engine e;
  static std::uniform_int_distribution<unsigned> u(0, 9);

  std::vector<unsigned> ret;
  for (size_t i = 0; i != 100; ++i) {
    ret.push_back(u(e));
  }
  return ret;
}

/* -------------------------------------------------------------------------- */

int main() {
  { std::cout << "Hello World!" << std::endl; }

  {
    std::tuple<size_t, size_t, size_t> threeD;
    std::tuple<std::string, std::vector<double>, int, std::list<int>> someVal(
        "constants", {3.14, 2.718}, 42, {0, 1, 2, 3, 4, 5});

    // tuple constructor is explicit; must use direct initialization
    std::tuple<size_t, size_t, size_t> threeD_{1, 2, 3};

    // std::tuple<const char *, int, double>
    auto item = std::make_tuple("0-999-78345-X", 3, 20.00);
    std::cout << std::get<0>(item) << std::endl;

    typedef decltype(item) trans;
    std::cout << std::tuple_size<trans>::value << std::endl;
    std::tuple_element<1, trans>::type cnt; // cnt is an int
  }

  {
    std::bitset<32> bitvec(1U);

    std::bitset<13> bitvec1(0xbeef); // 1111011101111
    std::bitset<20> bitvec2(0xbeef); // 00001011111011101111

    std::bitset<128> bitvec3(~0ULL); // 0..63 are one; 64...127 are zero

    std::bitset<32> bitvec4("1100"); // 2 and 3 are one, rest are zero

    std::string str("1111111000000011001101");
    std::bitset<8> bitvec5(str, 5, 4);           // 1100
    std::bitset<8> bitvec6(str, str.size() - 4); // 1101

    std::cout << bitvec5 << ' ' << bitvec6 << std::endl;

    unsigned long ul = bitvec3.to_ulong();
    std::cout << "ul = " << ul << std::endl;
  }

  {
    std::bitset<32> bitvec(1U);

    bool is_set = bitvec.any();
    bool is_not_set = bitvec.none();
    bool all_set = bitvec.all();

    size_t onBits = bitvec.count();
    size_t sz = bitvec.size();

    bitvec.flip();
    bitvec.reset();
    bitvec.set();
  }

  {
    std::default_random_engine e; // generate a sequence of random unsigned

    for (size_t i = 0; i < 10; ++i) {
      std::cout << e() << " ";
    }
    std::cout << std::endl;
  }

  {
    std::uniform_int_distribution<unsigned> u(0, 9);
    std::default_random_engine e;

    for (size_t i = 0; i < 10; ++i) {
      std::cout << u(e) << " ";
    }
    std::cout << std::endl;

    std::cout << "min: " << 0 << " max: " << RAND_MAX << std::endl;
    std::cout << "min: " << e.min() << " max: " << e.max() << std::endl;
  }

  {
    std::default_random_engine e1;
    std::default_random_engine e2(2147483646);

    std::default_random_engine e3;
    e3.seed(32767);
    std::default_random_engine e4(32767);

    for (size_t i = 0; i != 5; ++i) {
      if (e1() == e2()) {
        std::cout << "unseeded match at iteration: " << i << std::endl;
      }
      if (e3() == e4()) {
        std::cout << "seeded match at iteration: " << i << std::endl;
      }
    }

    std::default_random_engine e(time(0)); // <- use system time as seed
  }

  {
    std::default_random_engine e;
    std::uniform_real_distribution<double> u(0, 1);

    for (size_t i = 0; i != 10; ++i) {
      std::cout << u(e) << " ";
    }
    std::cout << std::endl;
  }

  // normal distribution
  {
    std::default_random_engine e;
    std::normal_distribution<> n(4, 1.5);

    std::vector<unsigned> vals(9);
    for (size_t i = 0; i != 200; ++i) {
      auto v = lround(n(e)); // round to the nearest integer
      if (0 <= v && v < vals.size()) {
        ++vals[v];
      }
    }
    for (size_t j = 0; j != vals.size(); ++j) {
      std::cout << j << ": " << std::string(vals[j], '#') << std::endl;
    }
  }

  // Bernoulli distribution
  {
    std::default_random_engine e;
    std::bernoulli_distribution b1;      // 50/50 by default
    std::bernoulli_distribution b2(.55); // 55/45

    auto res = b1(e); // always returns a bool value
  }

  {
    std::cout << "default bool values: " << true << " " << false
              << "\nalpha bool values: " << std::boolalpha << true << " "
              << false << std::endl;

    bool res = true;
    std::cout << std::boolalpha << res << std::noboolalpha << std::endl;
  }

  {
    std::cout << "default: " << 20 << " " << 1024 << std::endl;
    std::cout << "octal: " << std::oct << 20 << " " << 1024 << std::endl;
    std::cout << "hex: " << std::hex << 20 << " " << 1024 << std::endl;
    std::cout << "decimal: " << std::dec << 20 << " " << 1024 << std::endl;
  }
  {
    std::cout << std::showbase;
    std::cout << "default: " << 20 << " " << 1024 << std::endl;
    std::cout << "in octal: " << std::oct << 20 << " " << 1024 << std::endl;
    std::cout << "in hex: " << std::hex << 20 << " " << 1024 << std::endl;
    std::cout << "in decimal: " << std::dec << 20 << " " << 1024 << std::endl;
    std::cout << std::noshowbase;
  }

  {
    std::cout << "Precision: " << std::cout.precision()
              << ", Value: " << sqrt(2.0) << std::endl;

    std::cout.precision(12);
    std::cout << "Precision: " << std::cout.precision()
              << ", Value: " << sqrt(2.0) << std::endl;

    std::cout << std::setprecision(3);
    std::cout << "Precision: " << std::cout.precision()
              << ", Value: " << sqrt(2.0) << std::endl;

    std::cout << std::setprecision(6);
    std::cout << "default format: " << 100 * sqrt(2.0) << '\n'
              << "scientific: " << std::scientific << 100 * sqrt(2.0) << '\n'
              << "fixed decimal: " << std::fixed << 100 * sqrt(2.0) << '\n'
              << "hexadecimal: " << std::hexfloat << 100 * sqrt(2.0) << '\n'
              << "use defaults: " << std::defaultfloat << 100 * sqrt(2.0)
              << "\n\n";
  }

  {
    std::cout << 10.0 << std::endl;
    std::cout << std::showpoint << 10.0 << std::noshowpoint << std::endl;
    std::cout << std::endl;
  }

  {
    int i = -16;
    double d = 3.14159;

    std::cout << "i: " << std::setw(12) << i << "next col" << '\n'
              << "d: " << std::setw(12) << d << "next col" << '\n';

    std::cout << std::left;
    std::cout << "i: " << std::setw(12) << i << "next col" << '\n'
              << "d: " << std::setw(12) << d << "next col" << '\n';
    std::cout << std::right;

    std::cout << std::right;
    std::cout << "i: " << std::setw(12) << i << "next col" << '\n'
              << "d: " << std::setw(12) << d << "next col" << '\n';

    std::cout << std::internal;
    std::cout << "i: " << std::setw(12) << i << "next col" << '\n'
              << "d: " << std::setw(12) << d << "next col" << '\n';

    std::cout << std::setfill('#');
    std::cout << "i: " << std::setw(12) << i << "next col" << '\n'
              << "d: " << std::setw(12) << d << "next col" << '\n';

    std::cout << std::endl;
  }
}
