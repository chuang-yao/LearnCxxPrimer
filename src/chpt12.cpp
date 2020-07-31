//
//  Chapter 12 - Dynamic Memory Management
//

#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <string>
#include <vector>

using namespace std;

class StrBlob {
  friend class StrBlobPtr;

public:
  using size_type = vector<string>::size_type;

  // constructors
  StrBlob();
  StrBlob(initializer_list<string>);

  size_type size() const { return data->size(); }
  bool empty() const { return data->empty(); }

  // add and remove elements
  void push_back(const string &t) { data->push_back(t); }
  void pop_back();

  // element access
  string &front();
  string &back();
  const string &front() const;
  const string &back() const;

private:
  shared_ptr<vector<string>> data;
  void check(size_type i,
             const string &msg) const; // throw if data[i] is invalid
};

StrBlob::StrBlob() : data(make_shared<vector<string>>()) {}

StrBlob::StrBlob(initializer_list<string> il)
    : data(make_shared<vector<string>>(il)) {}

void StrBlob::check(size_type i, const string &msg) const {
  if (i >= data->size()) {
    throw out_of_range(msg);
  }
}

string &StrBlob::front() {
  check(0, "front on empty StrBlob");
  return data->front();
}

string &StrBlob::back() {
  check(0, "back on empty StrBlob");
  return data->back();
}

const string &StrBlob::front() const {
  check(0, "front on empty StrBlob");
  return data->front();
}

const string &StrBlob::back() const {
  check(0, "back on empty StrBlob");
  return data->back();
}

void StrBlob::pop_back() {
  check(0, "pop_back on empty StrBlob");
  return data->pop_back();
}

class StrBlobPtr {
public:
  StrBlobPtr() : curr(0) {}
  StrBlobPtr(StrBlob &a, size_t sz = 0) : wptr(a.data), curr(sz) {}

  string &deref() const;
  StrBlobPtr &incr();

private:
  weak_ptr<vector<string>> wptr;
  size_t curr;

  shared_ptr<vector<string>> check(size_t, const string &) const;
};

shared_ptr<vector<string>> StrBlobPtr::check(size_t i,
                                             const string &msg) const {
  auto ret = wptr.lock(); // is the vector still around?
  if (!ret) {
    throw runtime_error("unbound StrBlobPtr");
  }
  if (i >= ret->size()) {
    throw out_of_range(msg);
  }
  return ret;
}

string &StrBlobPtr::deref() const {
  auto p = check(curr, "dereference past end");
  return (*p)[curr];
}

StrBlobPtr &StrBlobPtr::incr() {
  check(curr, "increment past end of StrBlobPtr");
  ++curr;
  return *this;
}

int main() {
  { cout << "Hello World!" << endl; }

  {
    shared_ptr<string> p1;
    shared_ptr<list<int>> p2;

    shared_ptr<int> p3 = make_shared<int>(42);
    shared_ptr<string> p4 = make_shared<string>(10, '9');
    shared_ptr<int> p5 = make_shared<int>();

    auto p6 = make_shared<vector<string>>();
  }

  {
    auto p = make_shared<int>(42);
    auto q(p); // object to which p and q point has two users
    cout << q.use_count() << endl; // 2

    auto r = make_shared<int>(42);
    r = q; // object r had pointed to is automatically destroyed
  }

  {
    // Why should new and delete be avoided?
    // 1. considerably more error-prone than smart pointers
    // 2. cannot use the default definitions for copying, assigning and
    // destroying class objects

    int *pi = new int; // a dynamically allocated, unamed, uninitialized int
    string *ps = new string; // initialized to empty string

    delete pi;
    delete ps;
  }

  {
    int *pi = new int(42);
    string *ps = new string(10, '9');
    vector<int> *pv = new vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    delete pi;
    delete ps;
    delete pv;
  }

  {
    string *ps1 = new string;  // default initialized to empty string
    string *ps = new string(); // value initialized to empty string
    int *pi1 = new int;        // default initialized; *pi1 is undefined
    int *pi2 = new int();      // value initialized to 0; *pi2 is 0

    delete ps1;
    delete ps;
    delete pi1;
    delete pi2;
  }

  {
    auto p = new auto(42); // p1 is int*
    delete p;
  }

  {
    const int *pci = new const int(1024);
    const string *pcs = new const string;

    delete pci;
    delete pcs;
  }

  {
    int *p1 = new int;           // throws std::bad_alloc if failed
    int *p2 = new (nothrow) int; // returns a nullptr if failed

    delete p1;
    delete p2;
  }

  {
    int *p(new int(42));
    auto q = p;  // p and q point to the same memory
    delete p;    // invalidates both p and q
    p = nullptr; // indicates p no longer points to an object

    // cout << *q << endl; // undefined; q is a dangling pointer; be wary of
    // runtime error delete q; // delete the same memory twice may corrupt the
    // free store
    q = nullptr;
  }

  {
    StrBlob x = StrBlob({"Hello", "World"});
    cout << x.back() << endl;
  }

  {
    shared_ptr<double> p1;
    // shared_ptr<int> p2 = new int(42); // not allowed; constructor is explicit
    shared_ptr<int> p2(new int(42));
  }

  {
    shared_ptr<int> p(new int(42));
    int *q = p.get();
    // {
    //     shared_ptr<int> tmp(q);
    // }
    // block ends; q is destroyed; memory pointed to by q is freed
    int foo = *p;

    p.reset(new int(1024));
  }

  {
    unique_ptr<double> p1;
    unique_ptr<int> p2(new int(42)); // direct form initialization only
  }

  {
    unique_ptr<string> p1(new string("Stegosaurus"));

    unique_ptr<string> p2(p1.release());
    cout << *p2 << endl;

    unique_ptr<string> p3(new string("Trex"));
    p2.reset(p3.release());
  }

  {
    unique_ptr<int> p(new int(42));
    // auto q = p; // some deleted function can not be referenced
  }

  {
    int ix = 1024, *pi = &ix, *pi2 = new int(2048);
    using IntP = unique_ptr<int>;

    // IntP p0(ix);
    // IntP p1(pi);
    IntP p2(pi2);
    // IntP p3(&ix);
    IntP p4(new int(2048));
    // IntP p5(p2.get()); // unique_ptr does not support .get()

    cout << *p2 << endl;
  }

  {
    auto p = make_shared<int>(42);
    cout << p.use_count() << " "; // 1

    weak_ptr<int> wp(p);
    cout << p.use_count() << endl; // 1
  }

  {
    StrBlob b = StrBlob({"Hello", "World"});
    StrBlobPtr p(b, 0);

    cout << p.deref() << endl;
    cout << p.incr().deref() << endl;
    // cout << p.incr().deref() << endl; // dereference past the end
  }

  {
    using arrT = int[10];
    int *p = new arrT{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    delete[] p;
  }

  {
    unique_ptr<int[]> up(new int[10]);
    for (size_t i = 0; i != 10; ++i) {
      up[i] = i;
    }
    up.release();

    shared_ptr<int> sp(new int[10], [](int *p) { delete[] p; });
    for (size_t i = 0; i != 10; ++i) {
      *(sp.get() + i) = i;
    }
    sp.reset(); // use the lambda we supplied to free memory
  }
  /*
  {
      int n = 10;
      string * const p = new string[n];
      string s;
      string *q = p;
      while (cin >> s && q != p + n)
      {
          *q++ = s;
      }
      const size_t size = q - p;
      // use the array
      delete[] p;
  }
*/
  {
    allocator<string> alloc;
    int n = 10;
    auto const p = alloc.allocate(n); // allocate raw, unconstructed memory

    auto q = p;
    alloc.construct(q++);          // q is an empty string
    alloc.construct(q++, 10, 'c'); // q is "cccccccccc"
    alloc.construct(q++, "hi");    // q is "hi"

    while (q != p) {
      alloc.destroy(--q);
    }

    alloc.deallocate(q, n);
  }
}