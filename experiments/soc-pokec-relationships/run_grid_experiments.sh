#!/bin/bash

stop_server_script() {
    run_server_pid=$(pgrep "run_server.sh")
    if [ -n "$run_server_pid" ]; then
        pkill -P "$run_server_pid"
        echo "Stopped run_server.sh with PID: $run_server_pid"
        wait "$run_server_pid" 2>/dev/null
    else
        echo "No running run_server.sh found."
    fi
    stop_mdb_server
}

stop_mdb_server() {
    mdb_server_pid=$(pgrep "mdb-server")
    if [ -n "$mdb_server_pid" ]; then
        kill "$mdb_server_pid"
        echo "Stopped mdb-server with PID: $mdb_server_pid"
        wait "$mdb_server_pid" 2>/dev/null
    else
        echo "No running mdb-server found."
    fi
}


# Check if script parameter is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <MS_STRATEGY>"
    exit 1
fi

ms_strategy=$1

# Define parameters for both scripts
server_params=(start0 start1 start2)
declare -A query_params_dict
query_params_dict["start0"]="5"
query_params_dict["start1"]="4"
query_params_dict["start2"]="2"

# Create results directory with timestamp
timestamp=$(date +%Y-%m-%d_%H-%M-%S)
results_dir="results/experiment-$ms_strategy-$timestamp"
mkdir -p "$results_dir"

echo "Saving to $results_dir"

# Output files
execution_durations_file="$results_dir/$ms_strategy-execution-durations.csv"
visited_nodes_file="$results_dir/$ms_strategy-visited-nodes-counter.csv"
# query_times_file="$results_dir/$ms_strategy-query-times.csv"

# Clear previous results
echo "Server Param,Query Param,Execution Duration" > "$execution_durations_file"
echo "Server Param,Query Param,Visited Nodes Counter" > "$visited_nodes_file"

# Iterate over server parameters
for s in "${server_params[@]}"; do
    log_file="$results_dir/$ms_strategy-server_output_$s.log"

    stop_server_script

    sleep 1
    ./run_server.sh "$ms_strategy" "$s" data/databases/pokec_db > "$log_file" 2>&1 &
    sleep 1

    # Iterate over query parameters
    for q in ${query_params_dict[$s]}; do
        echo "Executing ./run_max_path_query.sh $q;"
        { time ./run_max_path_query.sh "$q"; } >/dev/null 2>> "$results_dir/time_output.txt"
        query_time=$(cat "$results_dir/time_output.txt" | tr '\n' ' ')
        echo "Executed query with param $q while server param was $ms_strategy $s"
    done
    sleep 2
    stop_server_script
    grep -oE 'Finished bfs with counter: Counter \[visited-nodes-counter\]: [0-9]+' "$log_file" | awk -v s="$s" -v q="$q" '{print s","NR","$NF}' >> "$visited_nodes_file"
    grep -oE 'Execution duration: [0-9]+' "$log_file" | awk -v s="$s" -v q="$q" '{print s","NR","$NF}' >> "$execution_durations_file"

done

python transform_experiment_results.py $execution_durations_file
python transform_experiment_results.py $visited_nodes_file
