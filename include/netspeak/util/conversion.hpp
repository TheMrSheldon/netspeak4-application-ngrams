// conversion.hpp -*- C++ -*-
// Copyright (C) 2011-2013 Martin Trenkmann
#ifndef NETSPEAK_UTIL_CONVERSION_HPP
#define NETSPEAK_UTIL_CONVERSION_HPP

#include <cxxabi.h>

#include <cstdlib>
#include <format>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

/**
 * Functions to convert numbers to strings and vice versa.
 */
namespace netspeak::util {

// http://stackoverflow.com/questions/12877521/human-readable-type-info-name
inline const std::string demangle(const char* mangled) {
  int status;
  std::unique_ptr<char[], void (*)(void*)> result(abi::__cxa_demangle(mangled, 0, 0, &status), std::free);
  return result.get() ? std::string(result.get()) : "error occurred";
}

template <typename T>
bool to_number(const std::string& value, T& number) {
  return std::istringstream(value) >> number;
}

template <typename T>
const T to_number(const std::string& value) {
  T number;
  if (!to_number<T>(value, number))
    throw std::invalid_argument(std::format("to_number failed : {}", value));
  return number;
}

template <typename T>
std::string to_string(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

} // namespace netspeak::util

#endif // NETSPEAK_UTIL_CONVERSION_HPP
