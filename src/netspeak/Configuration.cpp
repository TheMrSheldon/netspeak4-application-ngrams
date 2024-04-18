#include <netspeak/Configuration.hpp>

#include <string>

#include <filesystem>

#include <netspeak/util/traceable_error.hpp>

namespace netspeak {

namespace fs = std::filesystem;


// CONSTANTS


#define PREFIX const std::string Configuration

PREFIX::PATH_TO_HOME("path.to.home");
PREFIX::PATH_TO_PHRASE_INDEX("path.to.phrase-index");
PREFIX::PATH_TO_PHRASE_CORPUS("path.to.phrase-corpus");
PREFIX::PATH_TO_PHRASE_DICTIONARY("path.to.phrase-dictionary");
PREFIX::PATH_TO_POSTLIST_INDEX("path.to.postlist-index");
PREFIX::PATH_TO_HASH_DICTIONARY("path.to.hash-dictionary");
PREFIX::PATH_TO_REGEX_VOCABULARY("path.to.regex-vocabulary");
PREFIX::EXTENDS("extends");

PREFIX::CORPUS_KEY("corpus.key");
PREFIX::CORPUS_NAME("corpus.name");
PREFIX::CORPUS_LANGUAGE("corpus.language");

PREFIX::CACHE_CAPACITY("cache.capacity");

PREFIX::QUERY_LOWER_CASE("query.lower-case");

PREFIX::SEARCH_MAX_NORM_QUERIES("search.max-norm-queries");

PREFIX::SEARCH_REGEX_MAX_MATCHES("search.regex.max-matches");
PREFIX::SEARCH_REGEX_MAX_TIME("search.regex.max-time");

PREFIX::DEFAULT_PHRASE_INDEX_DIR_NAME("phrase-index");
PREFIX::DEFAULT_PHRASE_CORPUS_DIR_NAME("phrase-corpus");
PREFIX::DEFAULT_PHRASE_DICTIONARY_DIR_NAME("phrase-dictionary");
PREFIX::DEFAULT_POSTLIST_INDEX_DIR_NAME("postlist-index");
PREFIX::DEFAULT_HASH_DICTIONARY_DIR_NAME("hash-dictionary");
PREFIX::DEFAULT_REGEX_VOCABULARY_DIR_NAME("regex-vocabulary");


const std::unordered_set<std::string> PATHS{
  Configuration::PATH_TO_HOME,
  Configuration::PATH_TO_PHRASE_INDEX,
  Configuration::PATH_TO_PHRASE_CORPUS,
  Configuration::PATH_TO_PHRASE_DICTIONARY,
  Configuration::PATH_TO_POSTLIST_INDEX,
  Configuration::PATH_TO_HASH_DICTIONARY,
  Configuration::PATH_TO_REGEX_VOCABULARY,
};


// CONSTRUCTOR


void Configuration::load_extends() {
  if (config_.contains(EXTENDS)) {
    auto extends_path = fs::path(config_.get(EXTENDS));
    if (extends_path.empty()) {
      return;
    }
    if (!base_dir_.empty()) {
      extends_path = fs::absolute(base_dir_ / extends_path);
    }
    extends_ = std::make_unique<Configuration>(extends_path);
  }
}
void Configuration::desugar_home() {
  if (config_.contains(PATH_TO_HOME)) {
    auto home_path = fs::path(config_.get(PATH_TO_HOME));
    if (home_path.empty()) {
      return;
    }
    if (!base_dir_.empty()) {
      home_path = fs::absolute(base_dir_ / home_path);
    }

    auto set_default = [&](const std::string& key,
                           const std::string& default_value) {
      if (!config_.contains(key)) {
        config_[key] =
            fs::weakly_canonical(fs::absolute(home_path / default_value))
                .string();
      }
    };

    set_default(PATH_TO_PHRASE_CORPUS, DEFAULT_PHRASE_CORPUS_DIR_NAME);
    set_default(PATH_TO_PHRASE_DICTIONARY, DEFAULT_PHRASE_DICTIONARY_DIR_NAME);
    set_default(PATH_TO_PHRASE_INDEX, DEFAULT_PHRASE_INDEX_DIR_NAME);
    set_default(PATH_TO_POSTLIST_INDEX, DEFAULT_POSTLIST_INDEX_DIR_NAME);
    set_default(PATH_TO_HASH_DICTIONARY, DEFAULT_HASH_DICTIONARY_DIR_NAME);
    set_default(PATH_TO_REGEX_VOCABULARY, DEFAULT_REGEX_VOCABULARY_DIR_NAME);
  }
}

Configuration::Configuration(
    std::initializer_list<util::Config::initializer_list_type> list)
    : config_(list), extends_(), base_dir_() {
  load_extends();
  desugar_home();
}
Configuration::Configuration(fs::path file_name)
    : config_(file_name), extends_(), base_dir_(file_name.parent_path()) {
  load_extends();
  desugar_home();
}
Configuration::Configuration(const util::Config& config)
    : config_(config), extends_(), base_dir_() {
  load_extends();
  desugar_home();
}


// METHODS


std::string Configuration::get_required(const std::string& key) const {
  auto value = get_optional(key);
  if (value) {
    return *value;
  } else {
    throw util::tracable_runtime_error("The required key " + key +
                                       " is missing.");
  }
}
std::optional<std::string> Configuration::get_optional(
    const std::string& key) const {
  if (config_.contains(key)) {
    return config_.get(key);
  } else if (extends_) {
    return extends_->get_optional(key);
  } else {
    return std::nullopt;
  }
}
std::string Configuration::get(const std::string& key,
                               const std::string& defaultValue) const {
  auto value = get_optional(key);
  if (value) {
    return *value;
  } else {
    return defaultValue;
  }
}

fs::path relative_to(const std::string& path, const fs::path& to) {
  if (to.empty()) {
    return fs::weakly_canonical(path);
  } else {
    return fs::weakly_canonical(fs::absolute(to / path));
  }
}
fs::path Configuration::get_required_path(const std::string& key) const {
  return relative_to(get_required(key), base_dir_);
}
std::optional<fs::path> Configuration::get_optional_path(
    const std::string& key) const {
  const auto optional = get_optional(key);
  if (!optional) {
    return std::nullopt;
  }
  return relative_to(*optional, base_dir_);
}
fs::path Configuration::get_path(const std::string& key,
                                  const std::string& defaultValue) const {
  return relative_to(get(key, defaultValue), base_dir_);
}

bool parse_bool(const std::string& key, const std::string& value) {
  if (value == "false") {
    return false;
  }
  if (value == "true") {
    return true;
  }

  std::stringstream out;
  out << "Invalid boolean value for '" << key
      << "'. Expected 'true' or 'false' but found '" << value << "'.";
  throw util::tracable_runtime_error(out.str());
}
bool Configuration::get_required_bool(const std::string& key) const {
  return parse_bool(key, get_required(key));
}
std::optional<bool> Configuration::get_optional_bool(
    const std::string& key) const {
  const auto optional = get_optional(key);
  if (!optional) {
    return std::nullopt;
  }
  return parse_bool(key, *optional);
}
bool Configuration::get_bool(const std::string& key, bool defaultValue) const {
  return get_optional_bool(key).value_or(std::move(defaultValue));
}


std::unordered_set<std::string> Configuration::keys() const {
  std::unordered_set<std::string> result;
  if (extends_) {
    result = extends_->keys();
  }

  for (auto& foo : config_) {
    result.insert(foo.first);
  }

  result.erase(EXTENDS);

  return result;
}


util::Config Configuration::full_config() const {
  util::Config config;

  for (auto& key : keys()) {
    if (PATHS.find(key) == PATHS.end()) {
      // not a path
      config[key] = get_required(key);
    } else {
      // path
      config[key] = get_required_path(key).string();
    }
  }

  return config;
}


Configuration Configuration::extend(const Configuration& base) const {
  auto config = base.full_config();

  auto this_config = full_config();
  config.merge(this_config.begin(), this_config.end());

  return Configuration(config);
}

} // namespace netspeak
