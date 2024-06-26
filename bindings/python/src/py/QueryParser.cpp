#include "QueryParser.hpp"

#include <sstream>

#include "antlr4/parse.hpp"

#include "netspeak/Dictionaries.hpp"
#include "netspeak/regex/DefaultRegexIndex.hpp"
#include "netspeak/regex/RegexIndex.hpp"

namespace py {

NormQueryUnit::NormQueryUnit(NormQueryUnit::Kind kind, std::string text,
                             QueryUnitKind source_kind)
    : kind_(kind), text_(text), source_kind_(source_kind) {}

NormQueryUnit::Kind NormQueryUnit::get_kind() const {
  return kind_;
}
const std::string& NormQueryUnit::get_text() const {
  return text_;
}
NormQueryUnit::QueryUnitKind NormQueryUnit::get_source_kind() const {
  return source_kind_;
}


std::vector<NormQueryUnit>& NormQuery::get_units() {
  return units_;
}


netspeak::QueryNormalizer::InitConfig get_init_config(
    const std::string& vocab, const std::string& dictionary_csv,
    bool lower_case) {
  std::shared_ptr<netspeak::regex::RegexIndex> regex_index = nullptr;
  if (!vocab.empty()) {
    regex_index = std::make_shared<netspeak::regex::DefaultRegexIndex>(vocab);
  }

  std::shared_ptr<netspeak::Dictionaries::Map> dictionary = nullptr;
  if (!dictionary_csv.empty()) {
    std::istringstream iss(dictionary_csv);
    dictionary = std::make_shared<netspeak::Dictionaries::Map>(
        std::move(netspeak::Dictionaries::parse_csv(iss)));
  }

  return {
    .regex_index = regex_index,
    .dictionary = dictionary,
    .lower_case = lower_case,
  };
}
QueryParser::QueryParser(const std::string& vocab,
                         const std::string& dictionary_csv, bool lower_case)
    : normalizer(get_init_config(vocab, dictionary_csv, lower_case)) {}

NormQuery convert_norm_query(netspeak::model::NormQuery input) {
  NormQuery result;
  for (const auto& unit : input.units()) {
    NormQueryUnit::Kind kind = unit.tag();
    std::string text = *unit.text();
    NormQueryUnit::QueryUnitKind source_kind = unit.source().unit->tag();

    result.get_units().push_back(NormQueryUnit(kind, text, source_kind));
  }
  return result;
}
std::vector<NormQuery> QueryParser::parse(const std::string& query,
                                          QueryParserOptions options) {
  const auto parsed_query = antlr4::parse_query(query);
  std::vector<netspeak::model::NormQuery> norm_queries;
  normalizer.normalize(parsed_query,
                       {
                           .max_norm_queries = options.max_norm_queries,
                           .min_length = options.min_length,
                           .max_length = options.max_length,
                           .max_regex_matches = options.max_regex_matches,
                           .max_regex_time = std::chrono::milliseconds(
                               options.max_regex_time_ms),
                       },
                       norm_queries);

  // convert norm query types
  std::vector<NormQuery> result;
  for (const auto& nq : norm_queries) {
    result.push_back(convert_norm_query(nq));
  }

  return result;
}

} // namespace py
