// quintuple_traits.hpp -*- C++ -*-
// Copyright (C) 2011-2013 Martin Trenkmann
#ifndef NETSPEAK_VALUE_QUINTUPLE_TRAITS_HPP
#define NETSPEAK_VALUE_QUINTUPLE_TRAITS_HPP

#include "netspeak/value/quintuple.hpp"
#include "netspeak/value/value_traits.hpp"

namespace netspeak {
namespace value {

// -----------------------------------------------------------------------------
// Partial specialization for value::quintuple<T1, T2, T3, T4, T5>
// -----------------------------------------------------------------------------

template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct value_traits<quintuple<T1, T2, T3, T4, T5> > {

  typedef quintuple<T1, T2, T3, T4, T5> value_type;
  typedef uint16_t io_size_type;

  typedef value_traits<T1> T1Traits; // Compiles for arithmetic types only.
  typedef value_traits<T2> T2Traits; // Compiles for arithmetic types only.
  typedef value_traits<T3> T3Traits; // Compiles for arithmetic types only.
  typedef value_traits<T4> T4Traits; // Compiles for arithmetic types only.
  typedef value_traits<T5> T5Traits; // Compiles for arithmetic types only.

  static inline size_t size_of(const value_type& tuple) {
    return T1Traits::size_of(tuple.e1()) + T2Traits::size_of(tuple.e2()) +
           T3Traits::size_of(tuple.e3()) + T4Traits::size_of(tuple.e4()) +
           T5Traits::size_of(tuple.e5());
  }

  static inline char* copy_to(const value_type& tuple, char* buffer) {
    buffer = T1Traits::copy_to(tuple.e1(), buffer);
    buffer = T2Traits::copy_to(tuple.e2(), buffer);
    buffer = T3Traits::copy_to(tuple.e3(), buffer);
    buffer = T4Traits::copy_to(tuple.e4(), buffer);
    return T5Traits::copy_to(tuple.e5(), buffer);
  }

  static inline const char* copy_from(value_type& tuple, const char* buffer) {
    buffer = T1Traits::copy_from(tuple.e1(), buffer);
    buffer = T2Traits::copy_from(tuple.e2(), buffer);
    buffer = T3Traits::copy_from(tuple.e3(), buffer);
    buffer = T4Traits::copy_from(tuple.e4(), buffer);
    return T5Traits::copy_from(tuple.e5(), buffer);
  }

  static inline bool write_to(const value_type& tuple, FILE* file) {
    return T1Traits::write_to(tuple.e1(), file) &&
           T2Traits::write_to(tuple.e2(), file) &&
           T3Traits::write_to(tuple.e3(), file) &&
           T4Traits::write_to(tuple.e4(), file) &&
           T5Traits::write_to(tuple.e5(), file);
  }

  static inline bool read_from(value_type& tuple, FILE* file) {
    return T1Traits::read_from(tuple.e1(), file) &&
           T2Traits::read_from(tuple.e2(), file) &&
           T3Traits::read_from(tuple.e3(), file) &&
           T4Traits::read_from(tuple.e4(), file) &&
           T5Traits::read_from(tuple.e5(), file);
  }

  static inline void print_to(const value_type& tuple, std::ostream& os) {
    T1Traits::print_to(tuple.e1(), os);
    os << tuple_element_separator;
    T2Traits::print_to(tuple.e2(), os);
    os << tuple_element_separator;
    T3Traits::print_to(tuple.e3(), os);
    os << tuple_element_separator;
    T4Traits::print_to(tuple.e4(), os);
    os << tuple_element_separator;
    T5Traits::print_to(tuple.e5(), os);
  }

  static inline void println_to(const value_type& tuple, std::ostream& os) {
    print_to(tuple, os);
    os << '\n';
  }

  static inline void parse_from(value_type& tuple, std::istream& is) {
    T1Traits::parse_from(tuple.e1(), is);
    T2Traits::parse_from(tuple.e2(), is);
    T3Traits::parse_from(tuple.e3(), is);
    T4Traits::parse_from(tuple.e4(), is);
    T5Traits::parse_from(tuple.e5(), is);
  }

  static inline std::string type_name() {
    return T1Traits::type_name() + T2Traits::type_name() +
           T3Traits::type_name() + T4Traits::type_name() +
           T5Traits::type_name();
  }
};

// -----------------------------------------------------------------------------
// Operators
// -----------------------------------------------------------------------------

template <typename T1, typename T2, typename T3, typename T4, typename T5>
std::ostream& operator<<(std::ostream& os,
                         const quintuple<T1, T2, T3, T4, T5>& tuple) {
  if (os)
    value_traits<quintuple<T1, T2, T3, T4, T5> >::print_to(tuple, os);
  return os;
}

} // namespace value
} // namespace netspeak

#endif // NETSPEAK_VALUE_QUINTUPLE_TRAITS_HPP
