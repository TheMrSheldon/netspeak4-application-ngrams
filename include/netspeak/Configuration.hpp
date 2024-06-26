#ifndef NETSPEAK_CONFIGURATION_HPP
#define NETSPEAK_CONFIGURATION_HPP

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "util/Config.hpp"

namespace netspeak {

class Configuration final {
private:
  util::Config config_;
  std::unique_ptr<Configuration> extends_;
  std::filesystem::path base_dir_;

  void load_extends();
  void desugar_home();

public:
  Configuration() : config_(), extends_(), base_dir_() {}
  Configuration(std::initializer_list<util::Config::initializer_list_type> list);
  explicit Configuration(std::filesystem::path file_name);
  explicit Configuration(const util::Config& config);

  std::string get_required(const std::string& key) const;
  std::optional<std::string> get_optional(const std::string& key) const;
  std::string get(const std::string& key, const std::string& defaultValue) const;

  std::filesystem::path get_required_path(const std::string& key) const;
  std::optional<std::filesystem::path> get_optional_path(const std::string& key) const;
  std::filesystem::path get_path(const std::string& key, const std::string& defaultValue) const;

  bool get_required_bool(const std::string& key) const;
  std::optional<bool> get_optional_bool(const std::string& key) const;
  bool get_bool(const std::string& key, bool defaultValue) const;

  std::unordered_set<std::string> keys() const;

  util::Config full_config() const;

  Configuration extend(const Configuration& base) const;

public:
  /** \todo These should be constexpr and should not be all-caps to avoid clashes with any macros **/
  static const std::string PATH_TO_HOME;
  static const std::string PATH_TO_PHRASE_INDEX;
  static const std::string PATH_TO_PHRASE_CORPUS;
  static const std::string PATH_TO_PHRASE_DICTIONARY;
  static const std::string PATH_TO_POSTLIST_INDEX;
  static const std::string PATH_TO_HASH_DICTIONARY;
  static const std::string PATH_TO_REGEX_VOCABULARY;
  static const std::string EXTENDS;

  static const std::string CORPUS_KEY;
  static const std::string CORPUS_NAME;
  static const std::string CORPUS_LANGUAGE;

  static const std::string CACHE_CAPACITY;

  static const std::string QUERY_LOWER_CASE;

  static const std::string SEARCH_MAX_NORM_QUERIES;

  static const std::string SEARCH_REGEX_MAX_MATCHES;
  static const std::string SEARCH_REGEX_MAX_TIME;

  static const std::string DEFAULT_PHRASE_INDEX_DIR_NAME;
  static const std::string DEFAULT_PHRASE_CORPUS_DIR_NAME;
  static const std::string DEFAULT_PHRASE_DICTIONARY_DIR_NAME;
  static const std::string DEFAULT_POSTLIST_INDEX_DIR_NAME;
  static const std::string DEFAULT_HASH_DICTIONARY_DIR_NAME;
  static const std::string DEFAULT_REGEX_VOCABULARY_DIR_NAME;
};

} // namespace netspeak

#endif
