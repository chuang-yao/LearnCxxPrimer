//
//  Chapter 16 - Templates and Generic Programming
//

#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

/* -------------------------------------------------------------------------- */

// uses references to const as parameters
//  - some classes cannot be copied
//  - more efficient when dealing with large objects
/*
template <typename T> int compare(const T &v1, const T &v2) {
  if (v1 < v2) { // uses `<` only
    return 1;
  }
  if (v2 < v1) { // some classes only support `<`
    return -1;
  }
  return 0;
}
 */

// also supports comparing pointers
/* template <typename T> int compare(const T &v1, const T &v2) {
  if (std::less<T>()(v1, v2)) {
    return -1;
  }
  if (std::less<T>()(v2, v1)) {
    return 1;
  }
  return 0;
}

template <typename T> T foo(T *p) {
  T tmp = *p;
  // ...
  return tmp;
}
 */

// `typename` and `class` can be used interchangeably here
template <typename T, class U> T calc(const T &, const U &);

// nontype parameters
template <unsigned N, unsigned M>
int compare(const char (&p1)[N], const char (&p2)[M]) {
  return strcmp(p1, p2);
}

template <typename T> inline T min(const T &, const T &);

/* -------------------------------------------------------------------------- */

// declaring Blob and BlobPtr are templates
template <typename> class Blob;
template <typename> class BlobPtr;

template <typename T> bool operator==(const Blob<T> &, const Blob<T> &);

template <typename T> class Blob {
  friend class BlobPtr<T>;
  friend bool operator==<T>(const Blob<T> &, const Blob<T> &);

public:
  typedef T value_type;
  // need ‘typename’ before ‘std::vector<T>::size_type’
  // because ‘std::vector<T>’ is a dependent scope
  typedef typename std::vector<T>::size_type size_type;

  // constructors
  Blob();
  Blob(std::initializer_list<T>);
  template <typename It> Blob(It b, It e);

  // number of elements in Blob
  size_type size() const { return data->size(); }
  bool empty() const { return data->empty(); }

  // add and remove elements
  void push_back(const T &t) { return data->push_back(t); }
  void push_back(T &&t) { return data->push_back(std::move(t)); }
  void pop_back();

  // element access
  T &front();
  T &back();
  const T &front() const;
  const T &back() const;

  T &operator[](size_type i);
  const T &operator[](size_type i) const;

private:
  std::shared_ptr<std::vector<T>> data;
  void check(size_type i, const std::string &msg) const;
};

/* ------------------------------ Constructors ------------------------------ */

template <typename T>
Blob<T>::Blob() : data(std::make_shared<std::vector<T>>()) {}

template <typename T>
Blob<T>::Blob(std::initializer_list<T> il)
    : data(std::make_shared<std::vector<T>>(il)) {}

/* -------------------------------------------------------------------------- */

template <typename T>
void Blob<T>::check(size_type i, const std::string &msg) const {
  if (i >= data->size()) {
    throw std::out_of_range(msg);
  }
}

/* -------------------------------------------------------------------------- */

template <typename T> void Blob<T>::pop_back() {
  check(0, "pop_back on empty Blob");
  data->pop_back();
}

template <typename T> T &Blob<T>::back() {
  check(0, "back on empty Blob");
  return data->back();
}

template <typename T> const T &Blob<T>::back() const {
  check(0, "back on empty Blob");
  return data->back();
}

template <typename T> T &Blob<T>::front() {
  check(0, "front on empty Blob");
  return data->front();
}

template <typename T> const T &Blob<T>::front() const {
  check(0, "front on empty Blob");
  return data->front();
}

template <typename T> T &Blob<T>::operator[](size_type i) {
  check(i, "subscript out of range");
  return (*data)[i];
}

template <typename T> const T &Blob<T>::operator[](size_type i) const {
  check(i, "subscript out of range");
  return (*data)[i];
}

/* -------------------------------------------------------------------------- */

template <typename T> class BlobPtr {
public:
  BlobPtr() : curr(0) {}
  BlobPtr(Blob<T> &a, std::size_t sz = 0) : wptr(a.data), curr(sz) {}

  T &operator*() const {
    auto p = check(curr, "dereference past end");
    return (*p)[curr];
  }

  BlobPtr &operator++();
  BlobPtr &operator--();
  BlobPtr operator++(int);
  BlobPtr operator--(int);

private:
  std::shared_ptr<std::vector<T>> check(std::size_t, const std::string &) const;
  std::weak_ptr<std::vector<T>> wptr;
  std::size_t curr;
};

/* -------------------------------------------------------------------------- */

template <typename T>
std::shared_ptr<std::vector<T>>
BlobPtr<T>::check(size_t i, const std::string &msg) const {
  auto ret = wptr.lock();
  if (!ret) {
    throw std::runtime_error("unbound BlobPtr");
  }
  if (i >= ret->size()) {
    throw std::out_of_range(msg);
  }
  return ret;
}

/* -------------------------------------------------------------------------- */

template <typename T> BlobPtr<T> &BlobPtr<T>::operator++() {
  check(curr, "increment past end of Blob");
  ++curr;
  return *this;
}

template <typename T> BlobPtr<T> BlobPtr<T>::operator++(int) {
  BlobPtr ret = *this;
  ++*this;
  return ret;
}

template <typename T> BlobPtr<T> &BlobPtr<T>::operator--() {
  --curr;
  check(curr, "decrement past the begin of Blob");
  return *this;
}

template <typename T> BlobPtr<T> BlobPtr<T>::operator--(int) {
  BlobPtr ret = *this;
  --*this;
  return ret;
}

/* -------------------------------------------------------------------------- */

// we can make a template type parameter a friend in c++11
template <typename Type> class Bar {
  friend Type;
  // ...
};

/* -------------------------------------------------------------------------- */

template <typename T> using twin = std::pair<T, T>;

template <typename T> using partNo = std::pair<T, unsigned>;

/* -------------------------------------------------------------------------- */

// each instantiation of Foo has its own instance of static members
template <typename T> class Foo {
public:
  static std::size_t count() { return ctr; }

private:
  static std::size_t ctr; // declaration
};

template <typename T> std::size_t Foo<T>::ctr = 0; // definition

/* -------------------------------------------------------------------------- */

// a template with default arguments

template <typename T, typename F = std::less<T>>
int compare(const T &v1, const T &v2, F f = F()) {
  if (f(v1, v2)) {
    return -1;
  }
  if (f(v2, v1)) {
    return 1;
  }
  return 0;
}

/* -------------------------------------------------------------------------- */

class DebugDelete {
public:
  DebugDelete(std::ostream &s = std::cerr) : os(s) {}

  template <typename T> void operator()(T *p) {
    os << "deleting unique_ptr" << std::endl;
    delete p;
  }

private:
  std::ostream &os;
};

// sample instantiations for member template
/*
void DebugDelete::operator()(int *p) const { delete p; }
void DebugDelete::operator()(std::string *p) const { delete p;}
 */

/* -------------------------------------------------------------------------- */

template <typename T>  // parameter list for the class template
template <typename It> // parameter list for the member template
Blob<T>::Blob(It b, It e) : data(std::make_shared<std::vector<T>>(b, e)) {}

/* -------------------------------------------------------------------------- */

template <typename T> T fobj(T x, T y) { return x; }
template <typename T> T fref(const T &x, const T &y) { return y; }

/* -------------------------------------------------------------------------- */

template <typename A, typename B>
int flexibleCompare(const A &v1, const B &v2) {
  if (v1 < v2) {
    return -1;
  }
  if (v2 < v1) {
    return 1;
  }
  return 0;
}

/*
template <typename T> std::ostream &print(std::ostream &os, const T &obj) {
  return os << obj;
}
 */

/* -------------------------------------------------------------------------- */

// the compiler cannot deduce T1, user must specify T1 in each call
template <typename T1, typename T2, typename T3> T1 sum(T2 x, T3 y) {
  T1 ret = x + y;
  return ret;
}

/* -------------------------------------------------------------------------- */

template <typename It> auto fcn(It beg, It end) -> decltype(*beg) {
  return *beg;
}

template <typename It>
auto fcn2(It beg, It end) ->
    typename std::remove_reference<decltype(*beg)>::type {
  return *beg;
}

template <typename It> auto fcn3(It beg, It end) -> decltype(*beg + 0) {
  return *beg;
}

/* -------------------------------------------------------------------------- */

template <typename T> void f1(T &) {}

template <typename T> void f2(const T &) {}

template <typename T> void f3(T &&) {}

template <typename T> void g(T &&val) {}

/* -------------------------------------------------------------------------- */

void f(int v1, int &v2) { std::cout << v1 << " " << ++v2 << std::endl; }

void f_(int &&i, int &j) { std::cout << i << ' ' << j << std::endl; }

template <typename F, typename T1, typename T2> void flip1(F f, T1 t1, T2 t2) {
  f(t2, t1);
}

template <typename F, typename T1, typename T2>
void flip2(F f, T1 &&t1, T2 &&t2) {
  f(t2, t1);
}

template <typename F, typename T1, typename T2>
void flip(F f, T1 &&t1, T2 &&t2) {
  f(std::forward<T2>(t2), std::forward<T1>(t1));
}

/* -------------------------------------------------------------------------- */

// can be called on essentially any type; less specialized
template <typename T> std::string debug_rep(const T &t) {
  std::ostringstream ret;
  ret << t;
  return ret.str();
}

template <typename T> std::string debug_rep(T *p) {
  std::ostringstream ret;
  ret << "pointer: " << p;
  if (p) {
    ret << " " << debug_rep(*p);
  } else {
    ret << "null pointer";
  }
  return ret.str();
}

std::string debug_rep(const std::string &s) { return '"' + s + '"'; }

/* --------------------------- Variadic Templates --------------------------- */

template <typename T, typename... Args>
void foo(const T &t, const Args &... rest) {}

template <typename... Args> void g(Args... args) {
  std::cout << sizeof...(Args) << std::endl;
  std::cout << sizeof...(args) << std::endl;
}

template <typename T> std::ostream &print(std::ostream &os, const T &t) {
  return os << t;
}

template <typename T, typename... Args>
std::ostream &print(std::ostream &os, const T &t, const Args &... rest) {
  os << t << ", ";
  return print(os, rest...); // <- recursively call the rest; genius!!!
}

class Student {
public:
  Student() = default;
  Student(const std::string &s) : name(s) {}
  ~Student() = default;

private:
  std::string name = "";
};

template <typename... Args>
std::ostream &errorMsg(std::ostream &os, const Args &... rest) {
  return print(os, debug_rep(rest)...);
}

/* ---------------------- A Simplified `vector` Class ---------------------- */

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

  template <class... Args> void emplace_back(Args &&...);

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

template <class... Args> inline void StrVec::emplace_back(Args &&... args) {
  chk_n_alloc();
  alloc.construct(first_free++, std::forward<Args>(args)...);
}

/* -------------------------------------------------------------------------- */

// T should be const char *, a pointer to const char
// compare() requires a reference to a const T
// -> the parameter type should be reference to a const pointer to a const char
template <>
int compare(const char *const &p1, const char *const &p2, std::less<char *>) {
  return strcmp(p1, p2);
}

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

/* -------------------------------------------------------------------------- */

// function template specialization

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

// class template specialization

template <typename T> struct Foo_ {
  Foo_(const T &t = T()) : mem(t) {}

  void Bar() {}

  T mem;
};

template <> void Foo_<int>::Bar() {}

/* -------------------------------------------------------------------------- */

int main() {
  {
    std::cout << compare(1, 0) << std::endl;

    std::vector<int> vec1{1, 2, 3}, vec2{4, 5, 6};
    std::cout << compare(vec1, vec2)
              << std::endl; // template argument deduction

    std::cout << compare("hi", "mom") << std::endl; // sets N to 3, M to 4
  }

  {
    Blob<int> ia;
    Blob<int> ia2 = {0, 1, 2, 3, 4};

    Blob<std::string> names;
    Blob<double> prices;
  }

  { Blob<std::string> articles = {"a", "an", "the"}; }

  // a member is instantiated ONLY if the program uses that member
  {
    // instantiates Blob<int> and Blob<int>::Blob(std::initializer_list<int>)
    Blob<int> squares = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // instantiates Blob<int>::size()
    for (std::size_t i = 0; i != squares.size(); ++i) {
      squares[i] = i * i; // instantiates Blob<int>::operator[](size_t)
    }
  }

  {
    // template<typename T> using twin = std::pair<T, T>;
    twin<std::string> authors;
    twin<int> win_loss;
    twin<double> area;

    // we can fix one or more of the template parameters
    // template<typename T> using partNo = std::pair<T, unsigned>;
    partNo<std::string> books;
    partNo<Blob<int>> intsPair;
  }

  {
    Foo<int> fi;

    auto ct = Foo<int>::count();
    ct = fi.count();
    std::cout << ct << std::endl;
  }

  {
    double *p = new double;
    DebugDelete d;
    d(p);

    int *ip = new int;
    DebugDelete()(ip);
  }

  {
    // override the deleter of a unique_ptr
    std::unique_ptr<int, DebugDelete> p(new int, DebugDelete());
    std::unique_ptr<std::string, DebugDelete> sp(new std::string,
                                                 DebugDelete());
  }

  {
    int ia[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<long> vi = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::list<const char *> w = {"now", "is", "the", "time"};

    // instantiates Blob<int>
    // instantiates Blob<int>::Blob(int *, int *)
    Blob<int> a1(std::begin(ia), std::end(ia));

    Blob<int> a2(vi.begin(), vi.end());
    Blob<std::string> a3(w.begin(), w.end());
  }

  {
    // template <typename T> T fobj(T, T);
    // template <typename T> T fref(const T &, const T &);

    std::string s1("a value");
    const std::string s2("another value");

    fobj(s1, s1); // calls fobj(string, string)
    fref(s1, s2); // calls fref(const string &, const string &)

    int a[10], b[42];
    fobj(a, b); // calls fobj(int *, int *)
    // fref(a, b); // array types do not match
  }

  {
    print(std::cout, 42);
    std::cout << std::endl;

    std::ofstream f("../data/output.txt");
    print(f, "This is a fine note.");
  }

  {
    std::cout << sum<std::string>(std::string("hello"), std::string("world"))
              << std::endl;
  }

  {
    // std::max(42, 3.14); // error: needs explicit template argument
    std::max<int>(42, static_cast<int>(3.14));
    std::max<double>(42, 3.14);
  }

  {
    /*
        template <typename It> auto fcn3(It beg, It end)->decltype(*beg + 0) {
          return *beg;
        }
     */
    std::vector<double> iv{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto ret = fcn3(iv.begin(), iv.end()); // double
    std::cout << ret << std::endl;

    std::vector<std::string> sv{"chuang", "calvin", "yao"};
    // std::cout << fcn3(sv.begin(), sv.end()) << std::endl; // error:
    // *indistinguishable compiler vomit*
  }

  {
    // pf1 points to the instantiation of compare with T bound to int
    int (*pf1)(const int &, const int &, std::less<int>) = compare;
  }

  {
    /*
        template <typename T> void f1(T &) {}
        template <typename T> void f2(const T &) {}
        template <typename T> void f3(T &&) {}
     */

    int i = 42;
    const int ci = 42;

    f1(i);  // T is int
    f1(ci); // T is const int
    // f1(42); // error: cannot bind lvalue reference to an rvalue

    f2(i);  // T is int
    f2(ci); // T is int
    f2(42); // const & can be bound to an rvalue

    f3(42); // T is int
    f3(i);  // T is int &
    f3(ci); // T is const int &
  }

  {
    // template <typename T> void g(T &&val) {}

    int i = 0;
    const int ci = i;

    g(i);      // T is int &
    g(ci);     // T is const int &
    g(i * ci); // T is int
  }

  {
    /*
        template <typename T>
        typename std::remove_reference<T>::type &&move(T &&t) {
          return static_cast<typename std::remove_reference<T>::type &&>(t);
        }
     */

    std::string s1("hi!"), s2;
    s2 = std::move(std::string("bye!")); // t is string &&; T is string
    s2 = std::move(s1);                  // t is string; T is string &
  }

  {
    int i, j;

    i = j = 0;
    f(i, j);                                                  // 0 1
    std::cout << "f(i, j) -> " << i << ' ' << j << std::endl; // j is changed

    i = j = 0;
    flip1(f, i, j); // 0 1
    std::cout << "flip1(f, i, j) -> " << i << ' ' << j
              << std::endl; // i is unchanged

    i = j = 0;
    flip2(f, i, j); // 0 1
    std::cout << "flip2(f, i, j) -> " << i << ' ' << j
              << std::endl; // i is changed to 1

    // flip2(f_, i, 42); // error
    // cannot bind rvalue reference of type ‘int&&’ to lvalue of type ‘int’

    flip(f_, i, 42);
    std::cout << "flip(f_, i, j) -> " << i << ' ' << j << std::endl;
  }

  {
    std::string s("hi");
    std::cout << debug_rep(s) << std::endl;

    // debug_rep(const std::string &)
    // debug_rep(std::string *) <- requires no conversion
    std::cout << debug_rep(&s) << std::endl;

    const std::string *sp = &s;

    // debug_rep(const std::string *&)
    // debug_rep(const std::string *) <- more specialized
    std::cout << debug_rep(sp) << std::endl;
  }

  {
    std::string s("hi");

    // debug_rep<std::string>(const std::string &)
    // debug_rep(const std::string &) <- non-template
    std::cout << debug_rep(s) << std::endl;
  }

  {
    // debug_rep(const T &); T is char[10]
    // debug_rep(T *); T is const char <- more specialized; exact match
    // debug_rep(const string &); requires user-defined conversion
    std::cout << debug_rep("hi world!") << std::endl;
  }

  {
    int i = 0;
    double d = 3.14;
    std::string s = "how now brown cow";

    foo(i, s, 42, d);
    foo(s, 42, "hi");
    foo(d, s);
    foo("hi");

    g(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  }

  {
    int i = 0;
    std::string s("Hello World!");
    print(std::cout, i, s, 42);
    std::cout << std::endl;

    // print(std::cout, i, Student("chuang")); // <~~ no match for `operator<<`
  }

  {
    StrVec sv{"Hello", "World", "!"};
    sv.emplace_back(std::string("!!!"));
    for (const auto &s : sv) {
      std::cout << s << " ";
    }
    std::cout << std::endl;
  }

  {
    const char *p1 = "hi", *p2 = "mom";
    compare(p1, p2);
    compare("hi", "mom");
  }

  {
    std::unordered_multiset<Sales_data> SDset;
    SDset.insert(Sales_data("123-234345-456", 5, 2.99));
    SDset.insert(Sales_data("321-432543-654", 2, 8.99));
    SDset.insert(Sales_data("456-567678-987", 6, 1.99));
    for (auto &&sd : SDset) {
      std::cout << sd << std::endl;
    }
  }

  {
    Foo_<std::string> fs;
    fs.Bar(); // instantiates Foo_<std::string>::Bar()

    Foo_<int> fi;
    fi.Bar(); // uses our specialization of Foo_<int>::Bar()
  }
}