#ifndef NETSPEAK_ERROR_HPP
#define NETSPEAK_ERROR_HPP

#include <inttypes.h>

#include <string>

#include "util/traceable_error.hpp"

namespace netspeak {

using util::tracable_logic_error;
using util::tracable_runtime_error;

struct invalid_query_error final : public tracable_runtime_error {
  explicit invalid_query_error(const std::string& what) : tracable_runtime_error(what) {}
  virtual ~invalid_query_error() override = default;
};


/**
 * @brief The query_error_message struct
 *
 * returns a standard error messages for an invalid query
 *
 * @todo This should be a namespace and the members should be constexpr.
 */
struct query_error_message {
  static const std::string too_many_words;
  static const std::string too_many_regex_in_orderset;
  static const std::string invalid_regexword;
  static const std::string too_deeply_nested;
  static const std::string too_complex(uint32_t complexity, uint32_t max_worst_case_complexity);
  static const std::string too_long_query(int queryLength, int queryMaxLength);
};

/** @todo This should be a namespace and the members should be constexpr. **/
struct error_message {
  static const std::string no_error;
  static const std::string invalid_query;
  static const std::string server_error;
  static const std::string unknown_error;
  // messages with no corresponding error code
  static const std::string cannot_create;
  static const std::string cannot_open;
  static const std::string does_not_exist;
  static const std::string is_not_empty;
};

} // namespace netspeak

#endif
