// ExternalMap.hpp -*- C++ -*-
// Copyright (C) 2011-2013 Martin Trenkmann
#ifndef NETSPEAK_BIGHASHMAP_EXTERNAL_MAP_HPP
#define NETSPEAK_BIGHASHMAP_EXTERNAL_MAP_HPP

#include <cmph.h>

#include <mutex>
#include <string>
#include <type_traits>

#include "../util/checksum.hpp"
#include "../util/logging.hpp"
#include "../util/systemio.hpp"
#include "../value/pair.hpp"
#include "../value/pair_traits.hpp"
#include "CmphMap.hpp"

namespace netspeak::bighashmap {

namespace fs = std::filesystem;

/**
 * This class implements a hash table with a minimal perfect hash function for
 * addressing buckets. The hash function itself is hold in memory, whereas the
 * table data is stored on disk. Together with the wrapper class
 * {@link BigHashMap} this makes it possible to build very large hash maps
 * with millions of keys.
 * Note that this class is designed to build hash maps completely in memory as
 * required by the CMPH library. Also filling the hash table happens in memory,
 * because writing value to disk randomly takes too much time due to seek
 * operations.
 * The class {@link BigHashMap} applies a hierarchical approach on very large
 * key sets that cannot be constructed in memory. It first subdivides the key
 * set to smaller key sets and builds a hash map for each of these sets.
 * Although this construction schema is a build-in feature of the CMPH library
 * it turns out that the BRZ algorithm is much more memory consuming.
 *
 * @author  martin.trenkmann@uni-weimar.de
 * @version $Id$
 */
template <typename ValueT, bool IsThreadSafe, typename Traits>
class ExternalMap : public CmphMap<ValueT, Traits> {
public:
  typedef CmphMap<ValueT, Traits> Base;
  typedef typename Base::Value Value;
  typedef typename Base::Checksum Checksum;
  typedef std::integral_constant<bool, IsThreadSafe> IsThreadSafeSwitch;

  ExternalMap() = delete;

  virtual ~ExternalMap() {
    util::fclose(data_);
  }

private:
  typedef value::pair<Checksum, Value> Entry;
  typedef value::value_traits<Entry> EntryTraits;

  ExternalMap(const fs::path& mph_file, const fs::path& dat_file) : Base(mph_file) {
    data_ = util::fopen(dat_file, "rb");
  }

  inline bool ReadEntry(uint64_t offset, Entry& entry, std::false_type) const {
    util::fseek(data_, offset, SEEK_SET);
    return EntryTraits::read_from(entry, data_);
  }

  // Thread-safe version.
  inline bool ReadEntry(uint64_t offset, Entry& entry, std::true_type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return ReadEntry(offset, entry, std::false_type());
  }

public:
  static ExternalMap* Open(const fs::path& idx_file) {
    /** \todo return a unique_ptr instead **/
    std::ifstream ifs(idx_file);
    if (!ifs) {
      throw std::runtime_error(std::format("Cannot open : {}", idx_file));
    }
    std::string mph_file;
    std::string dat_file;
    std::getline(ifs, mph_file);
    std::getline(ifs, dat_file);
    return new ExternalMap(idx_file.parent_path() / mph_file, idx_file.parent_path() / dat_file);
  }

  bool Get(const std::string& key, Value& value) {
    Entry entry;
    const uint64_t offset = Base::Hash(key) * EntryTraits::size_of(entry);

    // Seek to table entry on disk and read it.
    if (!ReadEntry(offset, entry, IsThreadSafeSwitch())) {
      DEBUG_LOG("Key", key);
      DEBUG_LOG("File offset", offset);
      throw std::runtime_error("EntryTraits::read_from failed");
    }
    // Compute checksum and reject value on mismatch.
    const Checksum checksum(util::hash<Checksum>(key));
    if (entry.e1() != checksum) {
      // This is no error, unknown keys normally cause a hash collision.
      DEBUG_LOG("Key", key);
      DEBUG_LOG("File offset", offset);
      DEBUG_LOG("Actual checksum", checksum);
      DEBUG_LOG("Expected checksum", entry.e1());
      return false;
    }
    value = entry.e2();
    return true;
  }

private:
  FILE* data_;
  mutable std::mutex mutex_;
};

} // namespace netspeak::bighashmap

#endif // NETSPEAK_BIGHASHMAP_EXTERNAL_MAP_HPP
