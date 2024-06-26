#ifndef NETSPEAK_PHRASE_CORPUS_HPP
#define NETSPEAK_PHRASE_CORPUS_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "invertedindex/ByteBuffer.hpp"
#include "model/Phrase.hpp"
#include "model/typedefs.hpp"
#include "util/FileDescriptor.hpp"
#include "util/StringIdMap.hpp"

namespace netspeak {

using namespace model;

/**
 * A corpus that, given the id of a phrase, retrieves the text (= list of words)
 * and the frequency of a phrase.
 *
 * The retrieved frequency is the actual frequency and not some modified proxy.
 */
class PhraseCorpus {
public:
  static const std::string txt_dir;
  static const std::string bin_dir;
  static const std::string vocab_file;
  static const std::string phrase_file;

  PhraseCorpus();
  PhraseCorpus(const PhraseCorpus&) = delete;
  explicit PhraseCorpus(const std::filesystem::path& phrase_dir);

  bool is_open() const;

  void open(const std::filesystem::path& phrase_dir);

  /**
   * @brief Returns the number of words of the longest phrase in the corpus.
   * This will return 0 if the corpus is empty.
   */
  Phrase::Id::Length max_length() const {
    return max_length_;
  };

  /**
   * Returns the number of phrases with the given length.
   */
  Phrase::Count::Local count_phrases(Phrase::Id::Length phrase_len) const;
  /**
   * Returns the number of unique words in all phrases of any length.
   */
  size_t count_vocabulary() const;

  bool contains(const std::string& word) const;
  bool contains(const WordId& id) const;

  std::vector<Phrase> read_phrases(const std::vector<Phrase::Id>& phrase_ids) const;

private:
  Phrase decode_(const char* buffer, Phrase::Id phrase_id) const;

  void init_vocabulary_(const std::filesystem::path& vocab_file);

  void open_phrase_files_(const std::filesystem::path& phrase_dir);


  /**
   * @brief This maps the id of a word to its string value and vise versa.
   */
  std::optional<util::StringIdMap<WordId>> id_map;

  /**
   * @brief A map from the length of a phrase to the file descriptor of the
   * binary phrase corpus file that hold all phrase information for that length
   * (n-gram class).
   */
  std::unordered_map<Phrase::Id::Length, util::FileDescriptor> fd_map;

  Phrase::Id::Length max_length_;
};

} // namespace netspeak

#endif // NETSPEAK_PHRASE_CORPUS_HPP
