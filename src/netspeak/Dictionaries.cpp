#include <fstream>
#include <netspeak/Dictionaries.hpp>
#include <netspeak/error.hpp>
#include <netspeak/util/check.hpp>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace netspeak {

const Dictionaries::Map Dictionaries::read_from_file(const std::filesystem::path& csv) {
  std::ifstream ifs(csv);
  util::check(ifs.is_open(), error_message::cannot_open, csv);
  return parse_csv(ifs);
}

const Dictionaries::Map Dictionaries::parse_csv(std::basic_istream<char, std::char_traits<char>>& stream) {
  Map dict;
  std::string line;
  std::vector<std::string> tokens;
  const auto predicate = std::bind2nd(std::equal_to<char>(), '\t');
  while (std::getline(stream, line)) {
    boost::split(tokens, line, predicate);
    if (tokens.size() < 2)
      continue;
    for (unsigned i = 1; i != tokens.size(); ++i) {
      dict.insert(std::make_pair(tokens.front(), tokens[i]));
    }
  }
  return dict;
}

} // namespace netspeak
