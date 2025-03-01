#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <ms_strategy> <ms_start_node_label> <db path>"
    exit 1
fi

# Assign arguments to variables
ms_strategy="$1"
ms_start_node_label="$2"
db_path="$3"

MS_STRATEGY="$ms_strategy" MS_START_LABEL="$ms_start_node_label" ../../build/standard/bin/mdb-server "$db_path"  --timeout 36000
