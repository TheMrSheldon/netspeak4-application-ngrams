// Builder.hpp -*- C++ -*-
// Copyright (C) 2011-2013 Martin Trenkmann
#ifndef NETSPEAK_BIGHASHMAP_BUILDER_HPP
#define NETSPEAK_BIGHASHMAP_BUILDER_HPP

#include <cmph.h>

#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../util/checksum.hpp"
#include "../util/conversion.hpp"
#include "../util/logging.hpp"
#include "../util/systemio.hpp"
#include "../value/pair.hpp"
#include "../value/value_traits.hpp"
#include "CmphMap.hpp"

namespace netspeak::bighashmap {

namespace fs = std::filesystem;

template <typename ValueT, typename TraitsT = value::value_traits<ValueT> >
class Builder {
  typedef ValueT Value;
  typedef TraitsT Traits;
  typedef typename CmphMap<Value, Traits>::Checksum Checksum;
  typedef value::pair<Checksum, Value> Entry;
  typedef value::value_traits<Entry> EntryTraits;

public:
  static const char key_value_delim = '\t';
  static const std::string index_file_name;

  Builder() = delete;

  static void Build(const fs::path& input_dir, const fs::path& output_dir, Algorithm algorithm = Algorithm::BDZ) {
    if (!fs::exists(output_dir)) {
      throw std::runtime_error(std::format("Does not exist : {}", output_dir.string()));
    }
    if (!fs::is_empty(output_dir)) {
      throw std::runtime_error(std::format("Is not empty : {}", output_dir.string()));
    }

    if (!fs::exists(input_dir)) {
      throw std::runtime_error(std::format("Input directory does exist : {}", input_dir.string()));
    }

    util::log("Performing key space partitioning on input files");
    const auto input_files = Partition(input_dir, output_dir);
    const auto idx_file = output_dir / index_file_name;
    std::ofstream ofs(idx_file);
    if (!ofs) {
      throw std::runtime_error(std::format("Cannot create : {}", idx_file.string()));
    }
    const auto num_hashtables = input_files.size();
    for (std::size_t i = 0; i != num_hashtables; ++i) {
      std::ostringstream oss;
      oss << "Building map for partition " << i + 1 << '/' << num_hashtables;
      util::log(oss.str());
      ofs << BuildMap(input_files[i], algorithm).filename().string() << '\n';
      fs::remove(input_files[i]);
    }
    ofs.close();
    util::log("Construction succeeded");
  }

private:
  static bool IsUnique(const fs::path& key_file) {
    // Set current locale to C to enable byte-wise UTF-8 string comparison.
    const std::string command = "export LC_ALL=C && sort " + key_file.string() + " | uniq -d";
    FILE* file = popen(command.c_str(), "r");
    if (file == nullptr) {
      throw std::runtime_error(std::format("Cannot access the result of : {}", command));
    }
    char line[PATH_MAX];
    std::string duplicate_keys;
    while (std::fgets(line, sizeof(line), file) != nullptr) {
      duplicate_keys += line;
    }
    if (!duplicate_keys.empty()) {
      util::log("Found following duplicate keys in", key_file);
      std::cerr << duplicate_keys;
    }
    std::fclose(file);
    return duplicate_keys.empty();
  }

  /**
   * Generates a minimal perfect hash function from the given key file.
   * Note that the entire key file will be loaded into memory.
   */
  static fs::path BuildHashFunction(const fs::path& key_file, Algorithm algorithm) {
    if (!IsUnique(key_file)) {
      throw std::runtime_error("Keys are not unique");
    }
    FILE* key_rfs = util::fopen(key_file, "r");
    cmph_io_adapter_t* source(cmph_io_nlfile_adapter(key_rfs));
    cmph_config_t* config(cmph_config_new(source));
    cmph_config_set_algo(config, static_cast<CMPH_ALGO>(algorithm));
    cmph_t* mphf(cmph_new(config));
    cmph_config_destroy(config);
    cmph_io_nlfile_adapter_destroy(source);
    util::fclose(key_rfs);
    if (mphf == nullptr) {
      throw std::runtime_error(std::format("Cannot generate MPHF from : {}", key_file));
    }
    const fs::path mph_fn(key_file.string() + ".mph");
    FILE* mph_wfs(util::fopen(mph_fn, "wb"));
    cmph_dump(mphf, mph_wfs);
    util::fclose(mph_wfs);
    cmph_destroy(mphf);
    return mph_fn;
  }

  static fs::path BuildHashTable(const fs::path& mph_file, const fs::path& rec_file) {
    FILE* mph_rfs(util::fopen(mph_file, "rb"));
    cmph_t* mph(cmph_load(mph_rfs));
    if (mph == nullptr) {
      throw std::runtime_error(std::format("Cannot load MPHF from : {}", mph_file));
    }
    std::ifstream ifs(rec_file);
    if (!ifs) {
      throw std::runtime_error(std::format("Cannot open : {}", rec_file));
    }
    // allocate hash table memory
    Entry entry;
    const size_t table_size(cmph_size(mph) * EntryTraits::size_of(entry));
    char* table(static_cast<char*>(std::malloc(table_size)));
    if (table == nullptr) {
      throw std::runtime_error(std::format("Cannot allocate memory : {}", table_size));
    }

    char* address;
    std::string key;
    cmph_uint32 hash;
    std::string line;
    std::istringstream iss;
    std::string::size_type delim;
    while (std::getline(ifs, line)) {
      delim = line.find(key_value_delim);
      if (delim == std::string::npos)
        continue;
      key = line.substr(0, delim);
      iss.str(line.substr(delim));
      Traits::parse_from(entry.e2(), iss);
      entry.set_e1(util::hash<Checksum>(key));
      hash = cmph_search(mph, key.c_str(), key.size());
      // write checksum and value into table
      address = table + hash * EntryTraits::size_of(entry);
      EntryTraits::copy_to(entry, address);
      iss.clear();
    }
    // write table to disk
    const fs::path dat_fn(rec_file.string() + ".dat");
    FILE* dat_wfs(util::fopen(dat_fn, "wb"));
    util::fwrite(table, 1, table_size, dat_wfs);
    // clean up
    ifs.close();
    std::free(table);
    cmph_destroy(mph);
    util::fclose(mph_rfs);
    util::fclose(dat_wfs);
    return dat_fn;
  }

  static fs::path BuildMap(const fs::path& input_file, Algorithm algorithm) {
    const fs::path key_file = CreateKeyFile(input_file);
    const fs::path mph_file = BuildHashFunction(key_file, algorithm);
    const fs::path dat_file = BuildHashTable(mph_file, input_file);
    const fs::path idx_file = input_file.string() + ".idx";
    std::ofstream ofs(idx_file);
    if (!ofs) {
      throw std::runtime_error(std::format("Cannot create : {}", idx_file));
    }
    ofs << mph_file.filename().string() << '\n' << dat_file.filename().string() << std::endl;
    ofs.close();
    fs::remove(key_file);
    return idx_file;
  }

  static fs::path CreateKeyFile(const fs::path& rec_file) {
    std::ifstream ifs(rec_file);
    if (!ifs) {
      throw std::runtime_error(std::format("Cannot open : {}", rec_file));
    }
    const fs::path key_file(rec_file.string() + ".keys");
    std::ofstream ofs(key_file);
    if (!ofs) {
      throw std::runtime_error(std::format("Cannot create : {}", key_file));
    }
    std::string line;
    std::string::size_type delim;
    while (std::getline(ifs, line)) {
      delim = line.find(key_value_delim);
      if (delim == std::string::npos)
        continue;
      ofs << line.substr(0, delim) << '\n';
    }
    ofs.close();
    ifs.close();
    return key_file;
  }

  static uint64_t CountRecords(const fs::path& input_dir) {
    // std::getline is up to 10% faster than
    // std::fgetc and std::iostream::get (tested)
    std::string line;
    std::ifstream ifs;
    uint64_t record_count(0);
    fs::directory_iterator end;
    for (fs::directory_iterator it(input_dir); it != end; ++it) {
      if (fs::is_regular_file(it->path()) && !util::is_hidden_file(it->path())) {
        ifs.open(it->path());
        if (!ifs) {
          throw std::runtime_error(std::format("Cannot open : {}", it->path()));
        }
        while (std::getline(ifs, line)) {
          if (!line.empty())
            ++record_count;
        }
        ifs.close();
      }
    }
    return record_count;
  }

  static std::vector<fs::path> Partition(const fs::path& input_dir, const fs::path& output_dir) {
    const uint32_t num_records_per_hashmap = 10000000;
    const uint64_t num_records_total = CountRecords(input_dir);
    const uint32_t num_maps = 1 + num_records_total / num_records_per_hashmap;

    // create record files for hash maps
    std::vector<fs::path> rec_paths;
    std::vector<std::ofstream*> rec_files;
    for (std::size_t i = 0; i != num_maps; ++i) {
      const auto new_suffix = '.' + std::to_string(i);
      const auto new_filename = input_dir.filename().replace_extension(fs::path(new_suffix));
      rec_paths.push_back(output_dir / new_filename);
      rec_files.push_back(new std::ofstream(rec_paths.back()));
      if (!rec_files.back()->is_open()) {
        throw std::runtime_error(std::format("Cannot create : {}", rec_paths.back()));
      }
    }
    // scatter records
    std::string key;
    std::string line;
    std::ifstream ifs;
    fs::directory_iterator end;
    std::string::size_type delim;
    for (fs::directory_iterator it(input_dir); it != end; ++it) {
      if (fs::is_regular_file(it->path()) && !util::is_hidden_file(it->path())) {
        ifs.open(it->path());
        if (!ifs) {
          throw std::runtime_error(std::format("Cannot open : {}", it->path()));
        }
        util::log("Processing", it->path());
        while (std::getline(ifs, line)) {
          delim = line.find(key_value_delim);
          if (delim == std::string::npos)
            continue;
          key = line.substr(0, delim);
          *rec_files[util::hash32(key) % num_maps] << line << '\n';
        }
        ifs.close();
      }
    }
    // clean up
    for (const auto ofs : rec_files) {
      ofs->close();
      delete ofs;
    }
    return rec_paths;
  }
};

template <typename ValueT, typename Traits>
const std::string Builder<ValueT, Traits>::index_file_name("map.idx");

} // namespace netspeak::bighashmap

#endif // NETSPEAK_BIGHASHMAP_BUILDER_HPP
