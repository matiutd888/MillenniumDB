#!/bin/bash

# Check if script parameter is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <MS_STRATEGY>"
    exit 1
fi

ms_strategy=$1

# Define parameters for both scripts
server_params=(start0 start1)
query_params=(1 2 3)

# Create results directory with timestamp
timestamp=$(date +%Y-%m-%d_%H-%M-%S)
results_dir="results/experiment-$timestamp"
mkdir -p "$results_dir"

# Output files
execution_durations_file="$results_dir/$ms_strategy-execution-durations.csv"
visited_nodes_file="$results_dir/$ms_strategy-visited-nodes-counter.csv"
query_times_file="$results_dir/$ms_strategy-query-times.csv"

# Clear previous results
echo "Server Param,Query Param,Execution Duration" > "$execution_durations_file"
echo "Server Param,Query Param,Visited Nodes Counter" > "$visited_nodes_file"

# Iterate over server parameters
for s in "${server_params[@]}"; do
    log_file="$results_dir/$ms_strategy-server_output_$s.log"

    ./run_server.sh "$ms_strategy" "$s" data/databases/pokec_db > "$log_file" 2>&1 &
    server_pid=$!
    echo "Started server with param $ms_strategy $s (PID: $server_pid)"

    # Iterate over query parameters
    for q in "${query_params[@]}"; do
        ./run_max_path_query.sh "$q"
        echo "Executed query with param $q while server param was $ms_strategy $s"
    done

    kill "$server_pid"
    echo "Stopped server with param $ms_strategy $s"
    wait "$server_pid" 2>/dev/null

    grep -oE 'Finished bfs with counter: Counter \[visited-nodes-counter\]: [0-9]+' "$log_file" | awk -v s="$s" -v q="$q" '{print s","q","$NF}' >> "$visited_nodes_file"
    grep -oE 'Execution duration: [0-9]+' "$log_file" | awk -v s="$s" -v q="$q" '{print s","q","$NF}' >> "$execution_durations_file"

done
