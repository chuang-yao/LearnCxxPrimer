#pragma once

#include <initializer_list>
#include <string>
#include <vector>

template <typename T> void print(std::vector<T>);

void print(const char *);

void print(const int *beg, const int *end);

void print(const int[], size_t);

int fact(int);

int myFact(int);

size_t countCalls(void);

void resetInt(int &);

void resetInt(int *);

void error_msg(std::initializer_list<std::string>);

int listSum(std::initializer_list<int>);

void swapInt(int &, int &);

char &get_val(std::string &, std::string::size_type);

// inline and constexpr functions are normally defined in headers

inline const std::string &shorterString(const std::string &s1,
                                        const std::string &s2) {
  return s1.size() <= s2.size() ? s1 : s2;
}

inline std::string &shorterString(std::string &s1, std::string &s2) {
  auto &r = shorterString(const_cast<const std::string &>(s1),
                          const_cast<const std::string &>(s2));
  return const_cast<std::string &>(r);
}
