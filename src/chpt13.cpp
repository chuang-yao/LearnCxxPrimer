//
//  Chapter 13 - Copy Control
//

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

struct X {
  X() { std::cout << "X()" << std::endl; }
  X(const X &) { std::cout << "X(const X &)" << std::endl; }
  X &operator=(const X &) {
    std::cout << "X &operator(const X &)" << std::endl;
    return *this;
  }
  ~X() { std::cout << "~X()" << std::endl; }
};

struct NoCopy {
  NoCopy() = default;
  NoCopy(const NoCopy &) = delete;
  NoCopy &operator=(const NoCopy &) = delete;
  ~NoCopy() = default;
};

class PrivateCopy {
private:
  PrivateCopy(const PrivateCopy &);
  PrivateCopy &operator=(const PrivateCopy &);

public:
  PrivateCopy() = default;
  ~PrivateCopy();
};

class Foo {
public:
  Foo();                       // default constructor
  Foo(const Foo &);            // copy constructor
  Foo &operator=(const Foo &); // copy-assignment operator
  ~Foo();                      // destructor
};

class Sales_data {
public:
  Sales_data() = default;
  Sales_data(const Sales_data &);
  Sales_data &operator=(const Sales_data &);
  ~Sales_data() = default;

private:
  std::string bookNo;
  int units_sold = 0;
  double revenue = 0.0;
};

// equivalent to the copy constructor the compiler synthesizes
Sales_data::Sales_data(const Sales_data &orig)
    : bookNo(orig.bookNo), units_sold(orig.units_sold), revenue(orig.revenue) {}

// equivalent to the synthesized copy-assignment operator
Sales_data &Sales_data::operator=(const Sales_data &rhs) {
  bookNo = rhs.bookNo;
  units_sold = rhs.units_sold;
  revenue = rhs.revenue;
  return *this;
}

// value-like version of HasPtr
class HasPtrV {
  friend void swap(HasPtrV &, HasPtrV &);

public:
  HasPtrV(const std::string &s = std::string())
      : ps(new std::string(s)), i(0) {}
  HasPtrV(const HasPtrV &p) : ps(new std::string(*p.ps)), i(p.i) {}
  HasPtrV(HasPtrV &&p) noexcept : ps(p.ps), i(p.i) { p.ps = nullptr; }
  HasPtrV &operator=(const HasPtrV &);
  HasPtrV &operator=(HasPtrV rhs) { // handles both copy and move assignment
    swap(*this, rhs);
    return *this;
  }
  ~HasPtrV() { delete ps; }

private:
  std::string *ps;
  int i;
};

HasPtrV &HasPtrV::operator=(const HasPtrV &rhs) {
  auto newp = new std::string(*rhs.ps);
  delete ps; // free old memory
  ps = newp; // copy data from rhs
  i = rhs.i;
  return *this;
}

inline void swap(HasPtrV &lhs, HasPtrV &rhs) {
  using std::swap;
  swap(lhs.ps, rhs.ps); // swap pointers, not string data
  swap(lhs.i, rhs.i);   // swap int members
}

// pointer-like version of HasPtr
class HasPtr {
  friend void swap(HasPtr &, HasPtr &);

public:
  HasPtr(const std::string &s = std::string())
      : ps(new std::string(s)), i(0), use(new std::size_t(1)) {}
  HasPtr(const HasPtr &p) : ps(p.ps), i(p.i), use(p.use) { ++*use; }
  HasPtr &operator=(const HasPtr &);
  HasPtr &operator=(HasPtr);
  ~HasPtr();

private:
  std::string *ps;
  int i;
  std::size_t *use; // how many objects share *ps
};

HasPtr::~HasPtr() {
  if (--*use == 0) { // if reference count goes to zero
    delete ps;
    delete use;
  }
}

HasPtr &HasPtr::operator=(const HasPtr &rhs) {
  ++*rhs.use; // handles self-assignment

  if (--*use == 0) {
    delete ps;
    delete use;
  }

  ps = rhs.ps;
  i = rhs.i;
  use = rhs.use;

  return *this;
}

inline void swap(HasPtr &lhs, HasPtr &rhs) {
  using std::swap;
  swap(lhs.ps, rhs.ps); // swap pointers, not string data
  swap(lhs.i, rhs.i);   // swap int members
}

HasPtr &HasPtr::operator=(HasPtr rhs) {
  swap(*this, rhs);
  return *this;
}

/* ------------------------- A Copy-Control Example ------------------------- */

class Message;
class Folder;

class Message {
  friend class Folder;
  friend void swap(Message &lhs, Message &rhs);

public:
  explicit Message(const std::string &str = "") : contents(str) {}
  Message(const Message &);            // copy constructor
  Message(Message &&);                 // move constructor
  Message &operator=(const Message &); // copy-assignment operator
  Message &operator=(Message &&);      // move-assignment operator
  ~Message();                          // destructor

  void save(Folder &);
  void remove(Folder &);

private:
  std::string contents;       // message texts
  std::set<Folder *> folders; // folders that contain the message

  void add_to_Folders(const Message &);
  void remove_from_Folders();
  void move_Folders(Message *);
};

class Folder {
  friend class Message;
  friend void swap(Message &lhs, Message &rhs);

private:
  std::set<Message *> messages;

  void addMsg(Message *m) { messages.insert(m); }
  void remMsg(Message *m) { messages.erase(m); }
};

void Message::save(Folder &f) {
  folders.insert(&f);
  f.addMsg(this);
}

void Message::remove(Folder &f) {
  folders.erase(&f);
  f.remMsg(this);
}

// add this Message to folders that point to m
void Message::add_to_Folders(const Message &m) {
  for (auto f : m.folders) {
    f->addMsg(this);
  }
}

// remove this Message from its corresponding Folders
void Message::remove_from_Folders() {
  for (auto f : folders) {
    f->remMsg(this);
  }
}

Message::Message(const Message &m) : contents(m.contents), folders(m.folders) {
  add_to_Folders(m);
}

Message::~Message() { remove_from_Folders(); }

Message &Message::operator=(const Message &rhs) {
  remove_from_Folders();
  contents = rhs.contents;
  folders = rhs.folders;
  add_to_Folders(rhs);
  return *this;
}

void swap(Message &lhs, Message &rhs) {
  using std::swap;
  // remove pointers to original folders
  for (auto f : lhs.folders) {
    f->remMsg(&lhs);
  }
  for (auto f : rhs.folders) {
    f->remMsg(&rhs);
  }
  // swap the contents
  swap(lhs.folders, rhs.folders);
  swap(lhs.contents, rhs.contents);
  // restore pointers to swapped folders
  for (auto f : lhs.folders) {
    f->addMsg(&lhs);
  }
  for (auto f : rhs.folders) {
    f->addMsg(&rhs);
  }
}

// move the Folder pointers from msg to this Message
void Message::move_Folders(Message *msg) {
  folders = std::move(msg->folders); // use set move assignment
  for (auto f : folders) {
    f->remMsg(msg);
    f->addMsg(this); // might throw bad_alloc exception
  }
  msg->folders.clear(); // ensure destroy msg is harmless
}

Message::Message(Message &&msg) : contents(std::move(msg.contents)) {
  move_Folders(&msg);
}

Message &Message::operator=(Message &&rhs) {
  if (this != &rhs) { // check for self-assignment
    remove_from_Folders();
    contents = std::move(rhs.contents);
    move_Folders(&rhs);
  }
  return *this;
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

/*
void StrVec::reallocate() {
  auto newcapacity = size() ? 2 * size() : 1;
  auto newdata = alloc.allocate(newcapacity);
  auto dest = newdata;
  auto elem = elements;
  for (size_t i = 0; i != size(); ++i) {
    alloc.construct(dest++, std::move(*elem++));
  }
  free();
  elements = newdata;
  first_free = dest;
  cap = elements + newcapacity;
}
 */

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

int f() { return 42; }

/* -------------------------------------------------------------------------- */

class IntVec {
public:
  IntVec(std::initializer_list<int>);
  IntVec sorted() &&;
  IntVec sorted() const &;

private:
  std::vector<int> data;
};

IntVec::IntVec(std::initializer_list<int> il) {
  for (auto &&e : il) {
    data.push_back(e);
  }
}

// `this` points to an rvalue, we can sort it in place
IntVec IntVec::sorted() && {
  std::cout << "sorted() &&" << std::endl;

  std::sort(data.begin(), data.end());
  return *this;
}

// `this` points to a const or lvalue, we cannot sort it in place
IntVec IntVec::sorted() const & {
  std::cout << "sorted() const &" << std::endl;

  // IntVec ret(*this);
  // std::sort(ret.data.begin(), ret.data.end());
  // return *this;

  // IntVec ret(*this);
  // return ret.sorted();

  return IntVec(*this).sorted();
}

/* -------------------------------------------------------------------------- */

int main() {
  { std::cout << "Hello World!" << std::endl; }

  {
    Sales_data *p = new Sales_data;           // built-in pointer
    auto p2 = std::make_shared<Sales_data>(); // smart pointer
    Sales_data item(*p); // copy constructor copies *p into item
    std::vector<Sales_data> vec;
    vec.push_back(*p2); // copies *p2 into a vector
    delete p;           // destructor called on object pointed to by p
  } // exit local scope; destructor called on item, p2 and vec

  {
    X x1;
    X x2 = x1;
    x1 = x2;
  }

  {
    Folder f1, f2;

    Message msg1("Hello World!");
    msg1.save(f1);

    Message msg2 = msg1;
    msg2.save(f2);

    msg1.remove(f1);
  }

  {
    StrVec v{"Alice", "Bob", "Calvin"};
    v.push_back("David"); // calls move constructor
    std::string s = "Eve";
    v.push_back(s); // calls copy constructor
    std::cout << v.size() << " " << v.capacity() << std::endl;
  }

  {
    int i = 42;
    int &r = i;
    // int &&r = i; // an rvalue reference cannot be bound to an lvalue
    // int &r2 = i * 42;
    // initial value of reference to non-const must be an lvalue
    const int &r3 = i * 42; // okay to bind a ref to const to an rvalue
    int &&rr2 = i * 42;
  }

  {
    int &&rr1 = 42; // literals are rvalues
    // int &&rr2 = rr1; // expression `rr1` is an lvalue

    int &&rr3 = std::move(rr1); // okay
  }

  {
    std::vector<int> vi(100);
    int &&r1 = f();
    int &r2 = vi[0];
    int &r3 = r1;
    int &&r4 = vi[0] * f();
  }

  {
    std::string s1 = "a value", s2 = "another";
    auto n = (s1 + s2).find('a');
  }

  {
    IntVec v{4, 3, 2, 1, 0};
    v.sorted();
    IntVec(v).sorted();
  }
}