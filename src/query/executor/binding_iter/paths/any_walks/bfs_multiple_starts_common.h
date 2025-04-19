#ifndef BFS_MULTIPLE_STARTS_COMMON_H
#define BFS_MULTIPLE_STARTS_COMMON_H
#include "graph_models/object_id.h"
#include "query/executor/binding_iter/paths/any_walks/search_state.h"
#include <boost/unordered/unordered_node_map.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

// For unordered set
struct objectid_hash {
  std::size_t operator()(const ObjectId &o) const { return o.id; }
};

struct searchnodeid_hash {
  std::size_t operator()(const std::pair<uint32_t, ObjectId> &p) const {
    return p.second.id ^ p.first;
  }
};

inline std::string get_starting_label_str() {
  char *ms_start_label_c_str = std::getenv("MS_START_LABEL");
  std::string ret = "start";
  if (ms_start_label_c_str != nullptr) {
    ret = ms_start_label_c_str;
  }
  return ret;
}

class Counter {
private:
  unsigned long long int count;
  std::string name;

public:
  // Constructor
  Counter(std::string counterName) : count(0), name(counterName) {}

  void reset() { count = 0; }

  // Method to increment counter
  void increment() { count++; }

  // Method to get the current count
  int getCount() const { return count; }

  // Overloaded ostream operator for printing
  friend std::ostream &operator<<(std::ostream &os, const Counter &c) {
    os << "Counter [" << c.name << "]: " << c.count;
    return os;
  }
};

// size_t estimate_map_memory(
// const boost::unordered_node_map<std::pair<uint32_t, ObjectId>,
// Paths::Any::SearchState, searchnodeid_hash> &map);

class StructureMemoryLogger {
private:
  struct log_data {
    std::vector<size_t> logs;
    std::optional<size_t> current_max_size;
  };

  std::unordered_map<std::string, log_data> structure_logs;

public:
  StructureMemoryLogger() = default;

  void log_size(const std::string &structure_name, size_t size_in_bytes) {
    log_data &data = structure_logs[structure_name];
    data.logs.push_back(size_in_bytes);

    if (!data.current_max_size.has_value() ||
        size_in_bytes > data.current_max_size.value()) {
      data.current_max_size = size_in_bytes;
    }
  }

  std::vector<size_t> get_all_logs(const std::string &structure_name) const {
    auto it = structure_logs.find(structure_name);
    if (it != structure_logs.end()) {
      return it->second.logs;
    } else {
      return {};
    }
  }

  std::optional<size_t> get_max_size(const std::string &structure_name) const {
    auto it = structure_logs.find(structure_name);
    if (it != structure_logs.end()) {
      return it->second.current_max_size;
    } else {
      return std::nullopt;
    }
  }

  void clear_logs(const std::string &structure_name) {
    auto it = structure_logs.find(structure_name);
    if (it != structure_logs.end()) {
      it->second.logs.clear();
      it->second.current_max_size.reset();
    }
  }

  void clear_all() { structure_logs.clear(); }
};

template <typename K, typename V, typename Hash>
size_t estimate_unordered_node_map_memory(
    const boost::unordered_node_map<K, V, Hash> &map) {
  // Base size of the map object itself (pointers, size, load factor, etc.)
  size_t map_object_size = sizeof(map);

  // Estimate size of the dynamically allocated nodes (one per element)
  size_t nodes_bytes = 0;
  if (map.size() > 0) {
    // Estimate the size of each node. Nodes in linked lists typically
    // contain K, V, and at least a 'next' pointer. Maybe also cached hash.
    // This is a GUESS!
    size_t node_overhead_guess =
        sizeof(void *); // Minimal guess for 'next' pointer
    // Could be higher, e.g., sizeof(void*) + sizeof(std::size_t) for cached
    // hash

    size_t estimated_node_size = sizeof(K) + sizeof(V) + node_overhead_guess;

    nodes_bytes = map.size() * estimated_node_size;
  }

  // Estimate size of the bucket array
  // The bucket array holds pointers (or indices/offsets) to the first node in
  // each chain.
  size_t bucket_array_bytes = 0;
  if (map.bucket_count() > 0) {
    // Estimate size of each entry in the bucket array. Usually a pointer.
    size_t bucket_entry_size_guess = sizeof(void *);
    bucket_array_bytes = map.bucket_count() * bucket_entry_size_guess;
  }

  // Total estimated memory
  return map_object_size + nodes_bytes + bucket_array_bytes;
}
#endif
