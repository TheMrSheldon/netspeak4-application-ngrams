// triple.hpp -*- C++ -*-
// Copyright (C) 2011-2013 Martin Trenkmann
#ifndef NETSPEAK_VALUE_TRIPLE_HPP
#define NETSPEAK_VALUE_TRIPLE_HPP

namespace netspeak::value {

template <typename T1, typename T2, typename T3>
class triple {
public:
  typedef T1 e1_type;
  typedef T2 e2_type;
  typedef T3 e3_type;

public:
  triple() : e1_(), e2_(), e3_() {}

  triple(const e1_type& e1, const e2_type& e2, const e3_type& e3) : e1_(e1), e2_(e2), e3_(e3) {}

  triple(const triple& rhs) : e1_(rhs.e1_), e2_(rhs.e2_), e3_(rhs.e3_) {}

  ~triple() {}

  e1_type& e1() {
    return e1_;
  }

  e2_type& e2() {
    return e2_;
  }

  e3_type& e3() {
    return e3_;
  }

  const e1_type& e1() const {
    return e1_;
  }

  const e2_type& e2() const {
    return e2_;
  }

  const e3_type& e3() const {
    return e3_;
  }

  void set_e1(const e1_type& e1) {
    e1_ = e1;
  }

  void set_e2(const e2_type& e2) {
    e2_ = e2;
  }

  void set_e3(const e3_type& e3) {
    e3_ = e3;
  }

  void set(const e1_type& e1, const e2_type& e2, const e3_type& e3) {
    e1_ = e1;
    e2_ = e2;
    e3_ = e3;
  }

private:
  e1_type e1_;
  e2_type e2_;
  e3_type e3_;
};

template <typename T1, typename T2, typename T3>
inline bool operator==(const triple<T1, T2, T3>& lhs, const triple<T1, T2, T3>& rhs) {
  return lhs.e1() == rhs.e1() && lhs.e2() == rhs.e2() && lhs.e3() == rhs.e3();
}

template <typename T1, typename T2, typename T3>
inline bool operator!=(const triple<T1, T2, T3>& lhs, const triple<T1, T2, T3>& rhs) {
  return !(lhs == rhs);
}

template <typename T1, typename T2, typename T3>
inline bool operator<(const triple<T1, T2, T3>& lhs, const triple<T1, T2, T3>& rhs) {
  return lhs.e1() < rhs.e1();
}

template <typename T1, typename T2, typename T3>
inline bool operator>(const triple<T1, T2, T3>& lhs, const triple<T1, T2, T3>& rhs) {
  return lhs.e1() > rhs.e1();
}

} // namespace netspeak::value

#endif // NETSPEAK_VALUE_TRIPLE_HPP
