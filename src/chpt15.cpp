//
//  Chapter 15 - Object Oriented Programming
//

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

/*----------------------------------------------------------------------------*/

class Quote {
public:
  Quote() = default;                         // memberwise default initialize
  Quote(const Quote &) = default;            // memberwise copy
  Quote(Quote &&) = default;                 // memberwise copy
  Quote &operator=(const Quote &) = default; // copy assignment
  Quote &operator=(Quote &&) = default;      // move assignment
  virtual ~Quote() = default;

  Quote(const std::string &book, double sales_price)
      : bookNo(book), price(sales_price) {}

  std::string isbn() const { return bookNo; }
  virtual double net_price(std::size_t n) const { return n * price; }
  virtual void debug() const {
    std::cout << bookNo << " " << price; // minimal formatting
  }

  // simulating virtual copy
  virtual Quote *clone() const & { return new Quote(*this); }
  virtual Quote *clone() && { return new Quote(std::move(*this)); }

private:
  std::string bookNo;

protected:
  double price = 0.0;
};

/*
class Bulk_quote : public Quote {
public:
  Bulk_quote() = default;
  Bulk_quote(const std::string &book, double p, std::size_t qty, double disc)
      : Quote(book, p), min_qty(qty), discount(disc) {}
  double net_price(std::size_t) const override;

  void debug() const override {
    std::cout << min_qty << " " << discount; // minimal formatting
  }

private:
  std::size_t min_qty = 0;
  double discount = 0.0;
};
 */

// class to hold the discount rate and quantity
// derived classes will implement specific pricing strats using these data
class Disc_quote : public Quote {
public:
  Disc_quote() = default;
  Disc_quote(const std::string &book, double p, std::size_t qty, double disc)
      : Quote(book, p), quantity(qty), discount(disc) {}
  double net_price(std::size_t) const override = 0; // pure virtual function

  void debug() const override {
    std::cout << quantity << " " << discount << std::endl;
  }
  std::pair<std::size_t, double> discount_policy() const {
    return {quantity, discount};
  }

protected:
  std::size_t quantity = 0;
  double discount = 0.0;
};

// refactored Bulk_quote class
class Bulk_quote : public Disc_quote {
public:
  using Disc_quote::Disc_quote; // "inherit"s constructors from Disc_quote
  // synthesizes default constructors for Disc_quote

  // Bulk_quote() = default;
  Bulk_quote(const std::string &book, double p, std::size_t qty, double disc)
      : Disc_quote(book, p, qty, disc) {}
  double net_price(std::size_t) const override;

  Bulk_quote *clone() const & override { return new Bulk_quote(*this); }
  Bulk_quote *clone() && override { return new Bulk_quote(std::move(*this)); }
};

// if the specified number of items are purchased, use the discounted price
double Bulk_quote::net_price(std::size_t cnt) const {
  if (cnt >= quantity) {
    return (1 - discount) * cnt * price;
  } else {
    return cnt * price;
  }
}

// calculate and print the price for the given number of copies, applying
// any discounts
double print_total(std::ostream &os, const Quote &item, std::size_t n) {
  double ret = item.net_price(n); // net_price is a virtual function
  os << "ISBN: " << item.isbn() << " # sold: " << n << " total due: " << ret
     << std::endl;
  return ret;
}

/*----------------------------------------------------------------------------*/

class Basket {
public:
  // this implementation still exposes pointers to users of the class
  // void add_item(const std::shared_ptr<Quote> &sale) { items.insert(sale); }

  void add_item(const Quote &sale) { // copy the given object
    items.insert(std::shared_ptr<Quote>(sale.clone()));
  }
  void add_item(Quote &&sale) { // move the given object
    items.insert(std::shared_ptr<Quote>(std::move(sale).clone()));
  }

  double total_receipt(std::ostream &) const;

private:
  // predicate required by multiset; strict weak ordering
  static bool compare(const std::shared_ptr<Quote> &lhs,
                      const std::shared_ptr<Quote> &rhs) {
    return lhs->isbn() < rhs->isbn();
  }
  // a multiset to hold multiple quotes, ordered by `compare`
  std::multiset<std::shared_ptr<Quote>, decltype(compare) *> items{compare};
};

double Basket::total_receipt(std::ostream &os) const {
  double sum = 0.0;
  for (auto iter = items.cbegin(); iter != items.cend();
       iter = items.upper_bound(*iter)) { // no need to iterate through 1 by 1
    sum += print_total(os, **iter, items.count(*iter));
  }
  os << "Total Sales: " << sum << std::endl;
  return sum;
}

/*----------------------------------------------------------------------------*/

class base {
public:
  base(const std::string &s = std::string()) : basename(s) {}
  std::string name() {
    std::cout << "base::name()" << std::endl;
    return basename;
  }
  virtual void print(std::ostream &os) {
    std::cout << "base::print()" << std::endl;
    os << basename << std::endl;
  }
  virtual ~base() = default;

private:
  std::string basename;
};

class derived : public base {
public:
  derived(const std::string &s = std::string(), const int &i = 0)
      : base(s), i(i) {}
  void print(std::ostream &os) {
    std::cout << "derived::print()" << std::endl;
    base::print(os);
    os << i << std::endl;
  }

private:
  int i;
};

/*----------------------------------------------------------------------------*/

class Base {
  friend class Pal;

public:
  void pub_mem() {} // public member
  void memfcn() {}
  virtual int fcn() { return 0; }

  virtual void olf(int) { std::cout << "int" << std::endl; }
  virtual void olf(double) { std::cout << "double" << std::endl; }
  virtual void olf(char) { std::cout << "char" << std::endl; }

protected:
  int prot_mem; // protected member

private:
  char priv_mem; // private member
};

class Sneaky : public Base {
  friend void clobber(Sneaky &);
  friend void clobber(Base &);
  int j;
};

void clobber(Sneaky &s) { s.j = s.prot_mem = 0; } // okay

// void clobber(Base &b) { b.prot_mem = 0; } // error

struct Pub_Derv : public Base {
  int f() { return prot_mem; }
  // char g() { return priv_mem; } // priv_mem is inaccessible
};

struct Priv_Derv : private Base {
  int f1() const { return prot_mem; }
};

struct Derived_from_Public : public Pub_Derv {
  int use_base() { return prot_mem; }
  // okay; Base::prot_mem is public in Pub_Derv
};

struct Derived_from_Private : public Priv_Derv {
  // int use_base() { return prot_mem; }
  // Base::prot_mem is private in Priv_Derv
};

class Pal { // friend of Base
public:
  int f(Base b) { return b.prot_mem; } // ok; Pal is a friend of Base
  // int f2(Sneaky s) { return s.j; } // error; Pal is not a friend of Sneaky
  int f3(Sneaky s) { return s.prot_mem; }
  // ok; Pal can access the Base part of Sneaky
};

class Derived : public Base {
public:
  void memfcn(int) {}

  using Base::olf; // making all `olf`s accessible to Derived objects
  void olf(int) override { std::cout << "Integer" << std::endl; }
};

/*
class Base {
  public:
  virtual int fcn();
};
 */

class D1 : public Base {
public:
  int fcn(int) { return 1; } // not virtual; hides fcn from the base
  virtual void f2() {}
};

class D2 : public D1 {
public:
  int fcn(int) { return 2; } // not virtual; hides D1::fcn(int)
  int fcn() { return 3; }    // overrides virtual Base::fcn()
  void f2() {}               // overrides virtual D1::f2()
};

/*----------------------------------------------------------------------------*/

class B {
public:
  B() = default;
  B(const B &) = delete;
};

class D : public B {}; // D inherits B's deleted copy constructor

/*----------------------------------------------------------------------------*/

int main() {
  { std::cout << "Hello World!" << std::endl; }

  {
    Quote item;       // base type object
    Bulk_quote bulk;  // derived type object
    Quote *p = &item; // p points to a Quote object
    p = &bulk;        // p points to the Quote part of bulk
    Quote &r = bulk;  // a Quote reference bound to the Quote part of bulk
  }

  {
    Quote base;
    // Bulk_quote *bulkP = &base;  // no conversion from base to derived
    // Bulk_quote &bulkRef = base; // no conversion from base to derived

    Bulk_quote bulk;
    Quote *itemP = &bulk; // okay; derived to base conversion
    // Bulk_quote &bulkRef = itemP; // no conversion from base to derived
  }

  {
    Bulk_quote bulk;
    Quote item(bulk); // uses Quote::Quote(const Quote &) constructor
    item = bulk;      // calls Quote::operator=(const Quote &)
  }

  {
    Quote base("0-201-82470-1", 50);
    base.debug();
    print_total(std::cout << std::endl, base, 1); // calls Quote::net_price

    Bulk_quote derived("0-201-82470-1", 50, 5, 0.19);
    derived.debug();
    print_total(std::cout << std::endl, derived,
                10); // calls Bulk_quote::net_price
  }

  {
    base b("Microsoft ");
    derived d("Windows", 10);

    b.print(std::cout);
    d.print(std::cout);
  }

  {
    std::cout << "\nExercise 15.14" << std::endl;

    base bobj("MacBook");
    derived dobj("Pro", 13);
    base *bp1 = &bobj;
    base *bp2 = &dobj; // points to the base part of derived
    base &br1 = bobj;
    base &br2 = dobj;

    bobj.print(std::cout); // base::print()
    dobj.print(std::cout); // derived::print()
    bp1->name();           // base::name()
    bp2->name();           // base::name()
    br1.print(std::cout);  // base::print()
    br2.print(std::cout);  // derived::print()
  }

  {
      // Disc_quote discounted;
      // cannot create object of an abstract base class
  }

  {
    Pub_Derv d1;
    Priv_Derv d2;
    d1.pub_mem();
    // d2.pub_mem(); // pub_mem is private in the derived class
  }

  {
    Bulk_quote bulk;
    Bulk_quote *bulkP = &bulk;
    Quote *itemP = &bulk;

    // name lookup happens at compile time
    // static types determine which methods are visible

    bulkP->discount_policy();
    // itemP->discount_policy(); // error: name lookup happens at compile time
  }

  {
    Base b;
    Derived d;

    b.memfcn();
    d.memfcn(10);
    // d.memfcn(); // error: memfcn() is hidden by memfcn(int)
    d.Base::memfcn();
  }

  {
    Base bobj;
    D1 d1obj;
    D2 d2obj;

    Base *bp1 = &bobj, *bp2 = &d1obj, *bp3 = &d2obj;
    D1 *d1p = &d1obj;
    D2 *d2p = &d2obj;

    bp1->fcn(); // calls Base::fcn()
    bp2->fcn(); // calls Base::fcn()
    bp3->fcn(); // calls D2::fcn()

    // bp2->f2(); // error: no member name f2
    d1p->f2(); // calls D1::f2()
    d2p->f2(); // calls D2::f2()

    Base *p1 = &d2obj;
    D1 *p2 = &d2obj;
    D2 *p3 = &d2obj;
    // p1->fcn(42); // Base does not have fcn that takes an int
    p2->fcn(42); // calls D1::fcn(int)
    p3->fcn(42); // calls D2::fcn(int)
  }

  {
    Derived d;
    Base *p = &d;

    p->olf(42);       // calls the overloaded version in Derived
    p->Base::olf(42); // calls the version in Base

    p->olf(3.14);
    p->olf('*');

    d.olf(2.71); // `using Base::olf;` in Derived class definition
  }

  {
    Quote *itemP = new Quote;
    delete itemP;           // destructor for Quote called
    itemP = new Bulk_quote; // dynamic binding
    delete itemP;           // destructor for Bulk_quote called
  }

  {
    D d; // D's synthesized uses B's default
    // D d2(d);            // B's copy constructor is deleted
    // D d3(std::move(d)); // implicitly uses D's deleted copy constructor
  }

  {
    std::vector<Quote> basket;
    basket.push_back(Quote("0-201-82470-1", 50));
    basket.push_back(Bulk_quote("0-201-54848-8", 50, 10, 0.25));

    // basket[1] is a Quote object, sliced down from a Bulk_quote object
    std::cout << basket.back().net_price(15) << std::endl;
  }

  // We generally stores pointers instead of objects in containers
  // when it comes to an inheritance hierarchy

  {
    std::vector<std::shared_ptr<Quote>> basket;
    basket.push_back(std::make_shared<Quote>("0-201-82470-1", 50));
    basket.push_back(
        std::make_shared<Bulk_quote>("0-201-54848-8", 50, 10, 0.25));

    std::cout << basket.back()->net_price(15) << std::endl;
  }

  {
    Basket basket;

    basket.add_item(Quote("0-201-82470-1", 50));
    basket.add_item(Quote("0-201-82470-1", 50));
    basket.add_item(Bulk_quote("0-201-54848-8", 50, 3, 0.25));
    basket.add_item(Quote("0-201-82470-1", 50));
    basket.add_item(Bulk_quote("0-201-54848-8", 50, 3, 0.25));
    basket.add_item(Bulk_quote("0-201-54848-8", 50, 3, 0.25));
    basket.add_item(Bulk_quote("0-201-54848-8", 50, 3, 0.25));

    basket.total_receipt(std::cout);
  }
}
