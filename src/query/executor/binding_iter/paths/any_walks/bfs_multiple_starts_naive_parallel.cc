#include "bfs_multiple_starts_naive_parallel.h"
#include <iostream>
#include "system/path_manager.h"

using namespace std;
using namespace Paths::Any;

template <bool MULTIPLE_FINAL>
void BFSMultipleStartsNaiveParallel<MULTIPLE_FINAL>::_begin(Binding& _parent_binding) {
    parent_binding = &_parent_binding;
    // first_next = true;
    
    //////////////////////////////////////////////////////////////
    // Override starting nodes using the nodes with label "start"
    //////////////////////////////////////////////////////////////
    start_nodes.clear();
    bool interruption = false;
    auto label_start = QuadObjectId::get_string("start").id;
    auto it = quad_model.label_node->get_range(&interruption, {label_start, 0}, {label_start, UINT64_MAX});

    std::cout << "starting nodes:\n";
    for (auto record = it.next(); record != nullptr; record = it.next())
    {
        ObjectId node((*record)[1]);
        std::cout << node << std::endl;
        start_nodes.push_back(node);
    }
    //////////////////////////////////////////////////////////////

    for (auto node : start_nodes)
    {
        ObjectId start_object_id = node.is_var() ? (*parent_binding)[node.get_var()] : node.get_OID();
       
        auto start_state = visited.emplace(automaton.start_state,
                                          start_object_id,
                                          nullptr,
                                          true,
                                          ObjectId::get_null(),
                                          start_object_id);
        open.push(start_state.first.operator->());
        
        SearchNodeId start_node_id{automaton.start_state, start_object_id};
        first_visit_q.push(start_node_id);
    }
    iter = make_unique<NullIndexIterator>();
}


template <bool MULTIPLE_FINAL>
void BFSMultipleStartsNaiveParallel<MULTIPLE_FINAL>::_reset() {
    // Empty open and visited
    queue<const MultiSourceSearchState *> empty;
    open.swap(empty);
    visited.clear();
    if (MULTIPLE_FINAL) {
        reached_final.clear();
    }
    first_visit_q = {};

    // Add starting states to open and visited
    for (auto node : start_nodes)
    {
        ObjectId start_object_id = node.is_var() ? (*parent_binding)[node.get_var()] : node.get_OID();
       
        auto start_state = visited.emplace(automaton.start_state,
                                          start_object_id,
                                          nullptr,
                                          true,
                                          ObjectId::get_null(),
                                          start_object_id);
        open.push(start_state.first.operator->());
        
        SearchNodeId start_node_id{automaton.start_state, start_object_id};
        first_visit_q.push(start_node_id);
    }
    iter = make_unique<NullIndexIterator>();
}


template <bool MULTIPLE_FINAL>
bool BFSMultipleStartsNaiveParallel<MULTIPLE_FINAL>::_next() {
    // Check if first state is final
    while (!first_visit_q.empty())
    {
        SearchNodeId curr_first_node = first_visit_q.front();
        first_visit_q.pop();

        // Return false if node does not exist in the database
        if (!provider->node_exists(curr_first_node.second.id)) {
            open.pop();
            return false;
        }

        // Starting state is solution
        if (automaton.is_final_state[automaton.start_state]) {
            auto reached_state = MultiSourceSearchState(automaton.start_state,
                                             curr_first_node.second,
                                             nullptr,
                                             true,
                                             ObjectId::get_null(),
                                            curr_first_node.second);
            if (MULTIPLE_FINAL) {
                reached_final.insert({curr_first_node.second.id, curr_first_node.second.id});
            }
            auto path_id = path_manager.set_path(visited.insert(reached_state).first.operator->(), path_var);
            parent_binding->add(path_var, path_id);
            parent_binding->add(end, curr_first_node.second);
            return true;
        }
    }

    while (open.size() > 0) {
        auto current_state = open.front();
        auto reached_final_state = expand_neighbors(*current_state);

        // Enumerate reached solutions
        if (reached_final_state != nullptr) {
            auto path_id = path_manager.set_path(reached_final_state, path_var);
            parent_binding->add(path_var, path_id);
            parent_binding->add(end, reached_final_state->node_id);
            return true;
        } else {
            // Pop and visit next state
            open.pop();
        }
    }
    return false;

}


template <bool MULTIPLE_FINAL>
const MultiSourceSearchState* BFSMultipleStartsNaiveParallel<MULTIPLE_FINAL>::expand_neighbors(const MultiSourceSearchState& current_state) {
    // Check if this is the first time that current_state is explored
    if (iter->at_end()) {
        current_transition = 0;
        // Check if automaton state has transitions
        if (automaton.from_to_connections[current_state.automaton_state].size() == 0) {
            return nullptr;
        }
        set_iter(current_state);
    }

    // Iterate over the remaining transitions of current_state
    // Don't start from the beginning, resume where it left thanks to current_transition and iter (pipeline)
    while (current_transition < automaton.from_to_connections[current_state.automaton_state].size()) {
        auto& transition = automaton.from_to_connections[current_state.automaton_state][current_transition];

        // Iterate over records until a final state is reached
        while (iter->next()) {
            MultiSourceSearchState next_state(transition.to,
                                   ObjectId(iter->get_reached_node()),
                                   &current_state,
                                   transition.inverse,
                                   transition.type_id,
                                   current_state.bfs_id);
            auto visited_state = visited.insert(next_state);

            // If next state was visited for the first time
            if (visited_state.second) {
                auto reached_state = visited_state.first;
                open.push(reached_state.operator->());

                // Check if new path is solution
                if (automaton.is_final_state[reached_state->automaton_state]) {
                    if (MULTIPLE_FINAL) {
                        auto node_reached_final = reached_final.find(current_state.bfs_id, reached_state->node_id.id);
                        if (node_reached_final == reached_final.end()) {
                            reached_final.insert(reached_state->node_id.id);
                            return reached_state.operator->();
                        }
                    } else {
                        return reached_state.operator->();
                    }
                }
            }
        }

        // Construct new iter with the next transition (if there exists one)
        current_transition++;
        if (current_transition < automaton.from_to_connections[current_state.automaton_state].size()) {
            set_iter(current_state);
        }
    }
    return nullptr;
}


template <bool MULTIPLE_FINAL>
void BFSMultipleStartsNaiveParallel<MULTIPLE_FINAL>::accept_visitor(BindingIterVisitor& visitor) {
    visitor.visit(*this);
}


template class Paths::Any::BFSMultipleStartsNaiveParallel<true>;
template class Paths::Any::BFSMultipleStartsNaiveParallel<false>;
