//
//  Chapter 14 - Overloaded Operators and Conversions
//

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
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

/* -------------------------------------------------------------------------- */

/* ------------------------------- StrVec ----------------------------------- */

class StrVec {
public:
  StrVec() : elements(nullptr), first_free(nullptr), cap(nullptr) {}
  StrVec(const StrVec &);
  StrVec(std::initializer_list<std::string>);
  StrVec(StrVec &&) noexcept;
  StrVec &operator=(const StrVec &);
  StrVec &operator=(StrVec &&) noexcept;
  ~StrVec();

  void push_back(const std::string &);
  void push_back(std::string &&);
  size_t size() const { return first_free - elements; }
  size_t capacity() const { return cap - elements; }
  std::string *begin() const { return elements; }
  std::string *end() const { return first_free; }

  StrVec &operator=(std::initializer_list<std::string>);
  std::string &operator[](std::size_t n) { return elements[n]; }
  const std::string &operator=(std::size_t n) const { return elements[n]; }

private:
  inline static std::allocator<std::string> alloc;
  void chk_n_alloc() {
    if (size() == capacity()) {
      reallocate();
    }
  }
  std::pair<std::string *, std::string *> alloc_n_copy(const std::string *,
                                                       const std::string *);
  void free();
  void reallocate();

  std::string *elements;
  std::string *first_free;
  std::string *cap;
};

void StrVec::push_back(const std::string &s) {
  chk_n_alloc();
  alloc.construct(first_free++, s);
}

void StrVec::push_back(std::string &&s) {
  chk_n_alloc();
  alloc.construct(first_free++, std::move(s));
}

std::pair<std::string *, std::string *>
StrVec::alloc_n_copy(const std::string *b, const std::string *e) {
  auto data = alloc.allocate(e - b);
  return {data, std::uninitialized_copy(b, e, data)};
}

void StrVec::free() {
  if (elements) {
    for (auto p = first_free; p != elements;) {
      alloc.destroy(--p);
    }
    alloc.deallocate(elements, cap - elements);
  }
}

StrVec::StrVec(const StrVec &s) {
  auto newdata = alloc_n_copy(s.begin(), s.end());
  elements = newdata.first;
  first_free = cap = newdata.second;
}

StrVec::~StrVec() { free(); }

StrVec &StrVec::operator=(const StrVec &rhs) {
  auto data = alloc_n_copy(rhs.begin(), rhs.end());
  free();
  elements = data.first;
  first_free = cap = data.second;
  return *this;
}

void StrVec::reallocate() {
  auto newcapacity = size() ? 2 * size() : 1;
  auto first = alloc.allocate(newcapacity);
  auto last = std::uninitialized_copy(std::make_move_iterator(begin()),
                                      std::make_move_iterator(end()), first);
  free();
  elements = first;
  first_free = last;
  cap = elements + newcapacity;
}

StrVec::StrVec(std::initializer_list<std::string> il) {
  auto data = alloc_n_copy(il.begin(), il.end());
  elements = data.first;
  first_free = cap = data.second;
}

StrVec::StrVec(StrVec &&s) noexcept
    : elements(s.elements), first_free(s.first_free), cap(s.cap) {
  s.elements = s.first_free = s.cap = nullptr;
}

StrVec &StrVec::operator=(StrVec &&rhs) noexcept {
  if (this != &rhs) {
    free();

    elements = rhs.elements;
    first_free = rhs.first_free;
    cap = rhs.cap;

    rhs.elements = rhs.first_free = rhs.cap = nullptr;
  }
  return *this;
}

StrVec &StrVec::operator=(std::initializer_list<std::string> il) {
  auto data = alloc_n_copy(il.begin(), il.end());
  free();
  elements = data.first;
  first_free = cap = data.second;
  return *this;
}

/* -------------------------------------------------------------------------- */

/* --------------------------------- StrBlob -------------------------------- */

class StrBlob {
  friend class StrBlobPtr;

public:
  using size_type = std::vector<std::string>::size_type;

  StrBlob();
  StrBlob(std::initializer_list<std::string>);

  size_type size() const { return data->size(); }
  bool empty() const { return data->empty(); }

  void push_back(const std::string &t) { data->push_back(t); }
  void pop_back();

  std::string &front();
  std::string &back();
  const std::string &front() const;
  const std::string &back() const;

private:
  std::shared_ptr<std::vector<std::string>> data;
  void check(size_type i, const std::string &msg) const;
};

StrBlob::StrBlob() : data(std::make_shared<std::vector<std::string>>()) {}

StrBlob::StrBlob(std::initializer_list<std::string> il)
    : data(std::make_shared<std::vector<std::string>>(il)) {}

void StrBlob::check(size_type i, const std::string &msg) const {
  if (i >= data->size()) {
    throw std::out_of_range(msg);
  }
}

std::string &StrBlob::front() {
  check(0, "front on empty StrBlob");
  return data->front();
}

std::string &StrBlob::back() {
  check(0, "back on empty StrBlob");
  return data->back();
}

const std::string &StrBlob::front() const {
  check(0, "front on empty StrBlob");
  return data->front();
}

const std::string &StrBlob::back() const {
  check(0, "back on empty StrBlob");
  return data->back();
}

void StrBlob::pop_back() {
  check(0, "pop_back on empty StrBlob");
  return data->pop_back();
}

/* -------------------------------------------------------------------------- */

/* ------------------------------- StrBlobPtr ------------------------------- */

class StrBlobPtr {
public:
  StrBlobPtr() : curr(0) {}
  StrBlobPtr(StrBlob &a, size_t sz = 0) : wptr(a.data), curr(sz) {}

  std::string &deref() const;
  StrBlobPtr &incr();

  StrBlobPtr &operator++();
  StrBlobPtr &operator--();
  StrBlobPtr operator++(int);
  StrBlobPtr operator--(int);
  std::string &operator*() const {
    auto p = check(curr, "dereference past end");
    return (*p)[curr];
  }
  std::string *operator->() const {
    return &this->operator*(); // &(*this).operator*()
  }

private:
  std::weak_ptr<std::vector<std::string>> wptr;
  size_t curr;

  std::shared_ptr<std::vector<std::string>> check(size_t,
                                                  const std::string &) const;
};

std::shared_ptr<std::vector<std::string>>
StrBlobPtr::check(size_t i, const std::string &msg) const {
  auto ret = wptr.lock();
  if (!ret) {
    throw std::runtime_error("unbound StrBlobPtr");
  }
  if (i >= ret->size()) {
    throw std::out_of_range(msg);
  }
  return ret;
}

std::string &StrBlobPtr::deref() const {
  auto p = check(curr, "dereference past end");
  return (*p)[curr];
}

StrBlobPtr &StrBlobPtr::incr() {
  check(curr, "increment past end of StrBlobPtr");
  ++curr;
  return *this;
}

StrBlobPtr &StrBlobPtr::operator++() {
  check(curr, "increment past end of StrBlobPtr");
  ++curr;
  return *this;
}

StrBlobPtr &StrBlobPtr::operator--() {
  --curr;
  check(curr, "decrement past begin of StrBlobPtr");
  return *this;
}

StrBlobPtr StrBlobPtr::operator++(int) {
  StrBlobPtr ret = *this;
  ++*this;
  return ret;
}

StrBlobPtr StrBlobPtr::operator--(int) {
  StrBlobPtr ret = *this;
  --*this;
  return ret;
}

/* -------------------------------------------------------------------------- */

struct absInt {
  int operator()(int val) const { return val < 0 ? -val : val; }
};

class PrintString {
public:
  PrintString(std::ostream &o = std::cout, char c = ' ') : os(o), sep(c) {}
  void operator()(const std::string &s) const { os << s << sep; }

private:
  std::ostream &os;
  char sep;
};

// function object class generated by a lambda
// `[](const string &s1, const string &s2) { return s1.size() < s2.size(); }`
class ShorterString {
public:
  bool operator()(const std::string &s1, const std::string &s2) const {
    return s1.size() < s2.size();
  }
};

// function object class class generated by a lambda (capture by value)
// `[sz](const string &s){ return s.size() >= sz; }`
class SizeComp {
public:
  SizeComp(size_t n) : sz(n) {}
  bool operator()(const std::string &s) const { return s.size() >= sz; }

private:
  size_t sz;
};

/* ------------------------------ int(int, int) ----------------------------- */

int add(int i, int j) { return i + j; }

auto mod = [](int i, int j) { return i % j; };

struct divide {
  int operator()(int denominator, int divisor) { return denominator / divisor; }
};

/* -------------------------------- SmallInt -------------------------------- */

class SmallInt {
  friend SmallInt operator+(const SmallInt &, const SmallInt &);

public:
  SmallInt(int i = 0) : val(i) {
    std::cout << "int->SmallInt" << std::endl;
    if (i < 0 || i > 255) {
      throw std::out_of_range("Bad SamllInt value");
    }
  }
  /*explicit*/ operator int() const {
    std::cout << "SmallInt->int" << std::endl;
    return val;
  }

private:
  std::size_t val;
};

SmallInt operator+(const SmallInt &lhs, const SmallInt &rhs) {
  SmallInt ret;
  ret.val = lhs.val + rhs.val;
  return ret;
}

/* -------------------------------------------------------------------------- */
/* 
struct B;
struct A {
  A() = default;
  A(const B &); // converts a B to an A
};
struct B {
  operator A() const; // also converts a B to an A
};
 */
/* -------------------------------------------------------------------------- */

int main() {
  { std::cout << "Hello World!" << std::endl; }

  {
    StrVec v{"Alice", "Bob", "Calvin"};
    v.push_back("David");
    std::string s = "Eve";
    v.push_back(s);
    std::cout << v[2] << " " << v.size() << " " << v.capacity() << std::endl;
  }

  {
    int i = -42;
    absInt absObj;
    int ui = absObj(i);
    std::cout << i << " " << ui << std::endl;
  }

  {
    PrintString PrintStringObj(std::cout, '?');
    PrintStringObj("Yes");

    PrintString printer;
    printer("Hello World!");

    PrintString errors(std::cerr, '\n');
    printer("This is an error message ...");

    std::cout << std::endl;
    std::vector<std::string> v{"Alice", "Bob", "Calvin"};
    std::for_each(v.begin(), v.end(), PrintString(std::cout, '*'));
  }

  {
    std::vector<std::string> words{"David", "Alice", "Eve", "Bob", "Calvin"};
    std::stable_sort(words.begin(), words.end(), ShorterString());
    std::cout << std::endl;
    std::for_each(words.begin(), words.end(), PrintString());
    std::cout << std::endl;
  }

  {
    std::plus<int> intAdd;
    std::negate<int> intNegate;

    int sum = intAdd(10, 20);
    sum = intNegate(intAdd(10, 20));
    sum = intAdd(10, intNegate(10));
  }

  {
    std::vector<std::string> svec{"David", "Alice", "Eve", "Bob", "Calvin"};
    std::sort(svec.begin(), svec.end(), std::greater<std::string>());
    std::for_each(svec.begin(), svec.end(), PrintString());
    std::cout << std::endl;
  }

  {
    std::vector<std::string *> nameTable;
    std::sort(nameTable.begin(), nameTable.end(), std::less<std::string *>());
  }

  {
    std::map<std::string, int (*)(int, int)> binops;

    int (*fp)(int, int) = add;
    binops.insert({"+", fp});
    binops.insert({"%", mod}); // ?

    std::cout << binops["%"](13, 5) << std::endl;
  }

  {
    int (*fp)(int, int) = add;
    std::function<int(int, int)> f1 = fp;
    std::function<int(int, int)> f2 = divide();
    std::function<int(int, int)> f3 = [](int i, int j) { return i * j; };

    std::cout << f1(4, 2) << " " << f2(4, 2) << " " << f3(4, 2) << std::endl;
  }

  {
    std::map<std::string, std::function<int(int, int)>> binops = {
        {"+", [](int i, int j) { return i + j; }},
        {"-", std::minus<int>()},
        {"/", divide()},
        {"*", [](int i, int j) { return i * j; }},
        {"%", mod}};

    binops["+"](10, 5);
    binops["-"](10, 5);
    binops["/"](10, 5);
    binops["*"](10, 5);
    binops["%"](10, 5);
  }

  {
    // SmallInt si(256); // throws std::out_of_range
    SmallInt si;
    si = 4;
    int i = static_cast<int>(si) + 3;
  }

  {
    SmallInt si = static_cast<int>(3.14); // double -> int -> SmallInt
    double d = static_cast<double>(static_cast<int>(si)) +
               2.71; // SmallInt -> int -> double
  }
/* 
  {
    A f(const A &);
    B b;
    // A a = f(b); // ambiguous conversion
    A a1 = f(b.operator A()); // uses B's conversion operator
    A a2 = f(A(b));           // uses A's conversion constructor
  }
 */
  {
    SmallInt s1, s2;
    SmallInt s3 = s1 + s2;
    // int i = s3 + 0; // ambiguous call
  }

  {
    SmallInt s1;
    // double d = s1 + 3.14; // ambiguous
    double d = static_cast<int>(s1) + 3.14;
  }
}