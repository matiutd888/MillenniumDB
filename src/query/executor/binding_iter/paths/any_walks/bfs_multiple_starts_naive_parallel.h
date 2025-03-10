
#pragma once

#include <memory>
#include <queue>
#include <type_traits>

#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/unordered/unordered_node_map.hpp>
#include <boost/unordered/unordered_node_set.hpp>

#include "query/executor/binding_iter.h"
#include "query/executor/binding_iter/paths/any_walks/search_state.h"
#include "query/executor/binding_iter/paths/index_provider/path_index.h"
#include "query/parser/paths/automaton/rpq_automaton.h"
#include "search_state_multiple_starts.h"
// struct objectid_hash
// {
//     std::size_t operator()(const ObjectId &o) const
//     {
//         return o.id;
//     }
// };
//
// struct searchnodeid_hash
// {
//     std::size_t operator()(const std::pair<uint32_t, ObjectId> &p) const
//     {
//         return p.second.id ^ p.first;
//     }
// };

namespace Paths {
namespace Any {

/*
BFSMultipleStartsNaiveParallel returns a single path to all
reachable nodes from a starting node, using BFS.
*/
template <bool MULTIPLE_FINAL>
class BFSMultipleStartsNaiveParallel : public BindingIter {
  using SearchNodeId = std::pair<uint32_t, ObjectId>;

private:
  // Attributes determined in the constructor
  VarId path_var;
  std::vector<Id> start_nodes;
  VarId end;
  const RPQ_DFA automaton;
  std::unique_ptr<IndexProvider> provider;

  // where the results will be written, determined in begin()
  Binding *parent_binding;

  // Set of visited MultiSourceSearchStates
  boost::unordered_node_set<MultiSourceSearchState,
                            std::hash<MultiSourceSearchState>>
      visited;

  // Queue for BFS. Pointers point to the states in visited
  std::queue<const MultiSourceSearchState *> open;

  // Iterator for current node expansion
  std::unique_ptr<EdgeIter> iter;

  // The index of the transition being currently explored
  uint_fast32_t current_transition;

  // true in the first call of next() and after a reset()
  std::queue<SearchNodeId> first_visit_q;

  // MATI reached_final should be a map starting_v -> [map of reached final
  // states] Template type for storing nodes reached with a final state
  typename std::conditional<
      MULTIPLE_FINAL, boost::unordered_flat_set<std::pair<int64_t, uint64_t>>,
      DummyPairSet>::type reached_final;

public:
  // Statistics
  uint_fast32_t idx_searches = 0;

  BFSMultipleStartsNaiveParallel(VarId path_var, std::vector<Id> start_nodes,
                                 VarId end, RPQ_DFA automaton,
                                 std::unique_ptr<IndexProvider> provider)
      : path_var(path_var), start_nodes(start_nodes), end(end),
        automaton(automaton), provider(std::move(provider)) {}

  void accept_visitor(BindingIterVisitor &visitor) override;
  void _begin(Binding &parent_binding) override;
  void _reset() override;
  bool _next() override;

  // Expand neighbors from current state
  const MultiSourceSearchState *
  expand_neighbors(const MultiSourceSearchState &current_state);

  void assign_nulls() override {
    parent_binding->add(end, ObjectId::get_null());
    parent_binding->add(path_var, ObjectId::get_null());
  }

  // Set iterator for current node + transition
  inline void set_iter(const MultiSourceSearchState &s) {
    // Get current transition object from automaton
    auto &transition =
        automaton.from_to_connections[s.automaton_state][current_transition];

    // Get iterator from custom index
    iter = provider->get_iter(transition.type_id.id, transition.inverse,
                              s.node_id.id);
    idx_searches++;
  }
};
} // namespace Any
}; // namespace Paths
