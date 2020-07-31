#include <iostream>
#include <string>

int main() {

  {
    // auto use the referenced object type for deduction
    int i = 0, &r = i;
    auto a = r; // a is an int

    // auto ordinarily ignores top-level const
    const int ci = i, &cr = ci;
    auto b = ci;  // b is an int
    auto c = cr;  // c is an int
    auto d = &i;  // d is an int*
    auto e = &ci; // e is a const int* (pointer to a const int)

    const auto f = ci;

    // if we want deduced-type to have a top-level const, we must say so
    // explicitly
    auto &g = ci; // g is a reference to a const int
    // auto &h = 42;  // initial value of reference to non-const must be an lva
    const auto &j = 42; // ok: we can bind a const reference to a literal

    auto k = ci, &l = i;
    auto &m = ci, *p = &ci;
    // auto &n = i, *p2 = &ci;  // 'auto' type is "const int" for this entity,
    // but was previously implied to be "int"

    a = 42;
    b = 42;
    c = 42;
    // d = 42;  //a value of type "int" cannot be assigned to an entity of type
    // "int *" e = 42;  //a value of type "int" cannot be assigned to an entity
    // of type "const int *" g = 42;  //g is a reference to a const int
  }

  {
    const int ci = 0, &cj = ci;
    decltype(cj) x = 0;
    decltype(cj) y = x;
    // decltype(cj) z;  //reference variable "z" requires an initializer
  }

  {
    int i = 42, *p = &i, &r = i;
    decltype(r + 0) b;
    // decltype(*p) c;  //reference variable "c" requires an initializer

    // decltype((i)) d;  // (i) yields a reference
    decltype(i) e;
  }

  {
    int a = 3, b = 4;
    decltype(a) c = a;
    decltype((b)) d = a;
    ++c;
    ++d;
    std::cout << a << " " << b << " " << c << " " << d << std::endl;
  }

  {
    int a = 3, b = 4;
    decltype(a) c = a;
    decltype(a = b) d = a; // WARNING! expression with side effects has no
                           // effect in an unevaluated context!
    std::cout << a << " " << b << " " << c << " " << d << std::endl;
  }

  {
    struct Sales_data {
      std::string bookNo;
      unsigned int units_sold = 0;
      double revenue = 0.0;
    };
  }

  // difference between auto and decltype
  {
    int i = 1;
    int *p = &i;
    decltype(*p) x = i; // x is a reference to an int
    x = 2;
    std::cout << i << std::endl;

    i = 1;
    auto y = *p; // y is just a normal int
    y = 2;
    std::cout << i << std::endl;
  }
}