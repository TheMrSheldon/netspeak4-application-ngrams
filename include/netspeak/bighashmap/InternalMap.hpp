// InternalMap.hpp -*- C++ -*-
// Copyright (C) 2011-2013 Martin Trenkmann
#ifndef NETSPEAK_BIGHASHMAP_INTERNAL_MAP_HPP
#define NETSPEAK_BIGHASHMAP_INTERNAL_MAP_HPP

#include <string>

#include "../util/checksum.hpp"
#include "../util/logging.hpp"
#include "../util/systemio.hpp"
#include "../value/pair.hpp"
#include "../value/pair_traits.hpp"
#include "CmphMap.hpp"

namespace netspeak::bighashmap {

namespace fs = std::filesystem;

/**
 * NOTES
 * Lookup the map via internal_map::find is thread-safe by design.
 * So there is no need to synchronize access in multithreaded code.
 */
template <typename ValueT, typename Traits>
class InternalMap : public CmphMap<ValueT, Traits> {
public:
  typedef CmphMap<ValueT, Traits> Base;
  typedef typename Base::Value Value;
  typedef typename Base::Checksum Checksum;

  InternalMap() = delete;

  virtual ~InternalMap() {
    std::free(data_);
  }

private:
  typedef value::pair<Checksum, Value> Entry;
  typedef value::value_traits<Entry> EntryTraits;

  InternalMap(const fs::path& mph_file, const fs::path& dat_file) : Base(mph_file) {
    const Entry entry;
    data_ = static_cast<char*>(std::calloc(this->size(), EntryTraits::size_of(entry)));
    if (data_ == nullptr) {
      throw std::runtime_error(std::format("Cannot allocate memory : {}", this->size() * EntryTraits::size_of(entry)));
    }
    FILE* dat_rfs = util::fopen(dat_file, "rb");
    util::fread(data_, EntryTraits::size_of(entry), this->size(), dat_rfs);
    util::fclose(dat_rfs);
  }

public:
  static InternalMap* Open(const fs::path& idx_file) {
    std::ifstream ifs(idx_file);
    if (!ifs) {
      throw std::runtime_error(std::format("Cannot open : {}", idx_file));
    }
    std::string mph_file;
    std::string dat_file;
    std::getline(ifs, mph_file);
    std::getline(ifs, dat_file);
    return new InternalMap(idx_file.parent_path() / mph_file, idx_file.parent_path() / dat_file);
  }

  bool Get(const std::string& key, Value& value) {
    Entry entry;
    const uint64_t offset = Base::Hash(key) * EntryTraits::size_of(entry);
    EntryTraits::copy_from(entry, data_ + offset);
    const Checksum checksum = util::hash<Checksum>(key);
    if (entry.e1() != checksum) {
      // This is no error, unknown keys normally cause a hash collision.
      DEBUG_LOG("Key", key);
      DEBUG_LOG("Memory offset", offset);
      DEBUG_LOG("Actual checksum", checksum);
      DEBUG_LOG("Expected checksum", entry.e1());
      return false;
    }
    value = entry.e2();
    return true;
  }

private:
  char* data_;
};

} // namespace netspeak::bighashmap

#endif // NETSPEAK_BIGHASHMAP_INTERNAL_MAP_HPP
