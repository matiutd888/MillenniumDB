#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <ms_strategy> <ms_start_node_label> <db path> <build directory>"
    exit 1
fi

ms_strategy="$1"
ms_start_node_label="$2"
db_path="$3"
build_dir="$4"

valid_ms_strategy=("naive" "normal" "naive_parallel" "optimized" "optimized_bitset" "optimized_no_return")
if [[ ! " ${valid_ms_strategy[@]} " =~ " ${ms_strategy} " ]]; then
    echo "Error: ms_strategy must be one of [naive, normal, naive_parallel, optimized, optimized_no_return]"
    exit 1
fi

MS_STRATEGY="$ms_strategy" MS_START_LABEL="$ms_start_node_label" ../../{$build_dir}/bin/mdb-server "$db_path"  --timeout 36000
