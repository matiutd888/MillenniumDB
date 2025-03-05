#pragma once

#include <memory>
#include <queue>
#include <type_traits>

#include "boost/unordered/unordered_flat_map_fwd.hpp"
#include "query/executor/binding_iter.h"
#include "query/executor/binding_iter/paths/any_walks/search_state.h"
#include "query/executor/binding_iter/paths/index_provider/path_index.h"
#include "query/parser/paths/automaton/rpq_automaton.h"
#include "search_state.h"
#include "search_state_multiple_starts.h"
#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/unordered/unordered_node_map.hpp>
#include <boost/unordered/unordered_node_set.hpp>

#include "bfs_multiple_starts_common.h"
#include "debug_mati.h"


namespace Paths {
namespace Any {

/*
BFSMultipleStartsOptimized returns a single path to all
reachable nodes from a starting node, using BFS.
*/
template <bool MULTIPLE_FINAL>
class BFSMultipleStartsOptimized : public BindingIter

{
  using SearchNodeId = std::pair<uint32_t, ObjectId>;

private:
  using bfs_id_bit_set = uint64_t;
  static constexpr int NUM_CONCURRENT_BFS = sizeof(bfs_id_bit_set);
  // Attributes determined in the constructor
  VarId path_var;
  std::vector<Id> start_nodes;
  VarId end;
  const RPQ_DFA automaton;
  std::unique_ptr<IndexProvider> provider;
  Counter visited_nodes_counter;

  // where the results will be written, determined in begin()
  Binding *parent_binding;

  // Optimized structures
  std::vector<ObjectId> bfss_ordered;
  int current_bfs_chunk = 0;
  int num_nodes_in_current_chunk = 0; 
  int num_chunks = 0;
  uint64_t bit_mask_for_current_chunk;
  // -------------

  // Maybe can tell to unroll loop:
  // https://stackoverflow.com/questions/16022362/how-to-tell-the-compiler-to-unroll-this-loop
  // TODO will have to do assignment using this
  // Try to optyimize the for loop using openmp
  // https://chatgpt.com/share/67c6385f-179c-800b-9b8d-8d6ec423dcf2
  // TODO also think about memory overhead. how to avoid it?
  //
  // Maybe instead of array it can just be a map so that its smaller
  // Or maybe map:
  // unordered_node_map<SearchNodeId, SearchState>
  // search_states[NUM_CONCURRENT_BFSS]; boost::unordered_node_map<SearchNodeId,
  // std::array<SearchState, NUM_CONCURRENT_BFS>> seen_optimized;
  boost::unordered_node_map<SearchNodeId, SearchState, searchnodeid_hash>
      search_states[NUM_CONCURRENT_BFS];
  // boost::unordered_node_map<
  //     ObjectId,
  //     boost::unordered_node_map<SearchNodeId, SearchState,
  //                               searchnodeid_hash>,
  //     objectid_hash>
  //     seen;

  boost::unordered_node_map<SearchNodeId, bfs_id_bit_set, searchnodeid_hash>
      bfss_that_reached_given_node;

  // visit and visit_next. Maybe we can use set instead of map and the values
  // (bfs ids) can be taken from `bfss_that_reached_given_node`. hmm. are the
  // searchNodeIds useful at all?
  boost::unordered_node_map<SearchNodeId, bfs_id_bit_set, searchnodeid_hash>
      bfses_to_be_visited;
  boost::unordered_node_map<SearchNodeId, bfs_id_bit_set, searchnodeid_hash>
      bfses_to_be_visited_next;

  std::queue<SearchNodeId> visit_q;
  std::queue<std::pair<SearchNodeId, int>> first_visit_q;

  // Iterator for current node expansion
  std::unique_ptr<EdgeIter> iter;

  // The index of the transition being currently explored
  uint_fast32_t current_transition;

  std::queue<SearchState*> search_states_for_current_iteration;

  typename std::conditional<
      MULTIPLE_FINAL, boost::unordered_flat_set<std::pair<int64_t, uint64_t>>,
      DummyPairSet>::type reached_final;
  


public:
  // Statistics
  uint_fast32_t idx_searches = 0;

  BFSMultipleStartsOptimized(VarId path_var, std::vector<Id> start_nodes,
                             VarId end_nodes, RPQ_DFA automaton,
                             std::unique_ptr<IndexProvider> provider)
      : path_var(path_var), start_nodes(start_nodes), end(end_nodes),
        automaton(automaton), provider(std::move(provider)),
        visited_nodes_counter("visited-nodes-counter") {

    _debug_mati() << "hello!" << std::endl;
  }

  void accept_visitor(BindingIterVisitor &visitor) override;
  void _begin(Binding &parent_binding) override;
  void _reset() override;
  bool _next() override;
  
  void set_new_bfs_chunk_and_reset();
  void single_reset() ;
  bool single_next() ;
  void prepare_structures_for_next_chunk_processing();
  // Expand neighbors from current state
  const SearchState *
  expand_neighbors(const SearchNodeId &current_state);

  void assign_nulls() override {
    parent_binding->add(end, ObjectId::get_null());
    parent_binding->add(path_var, ObjectId::get_null());
  }

  // Set iterator for current node + transition
  inline void set_iter(const SearchNodeId &s) {
    // Get current transition object from automaton
    auto &transition =
        automaton.from_to_connections[s.first][current_transition];

    // Get iterator from custom index
    iter = provider->get_iter(transition.type_id.id, transition.inverse,
                              s.second.id);
    idx_searches++;
  }
};

} // namespace Any
} // namespace Paths
