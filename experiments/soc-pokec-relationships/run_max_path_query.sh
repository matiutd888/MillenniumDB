#!/bin/bash

if [ -z "${MILLENIUM_DB_HOME_DIR}" ]; then
    MILLENIUM_DB_HOME_DIR="/home/mateusz/studia-current/mgr/MillenniumDB"
fi

# Check if the correct number of arguments is provided
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <max_number_of_nodes_in_path>" "<query_number_to_run>"
    exit 1
fi

max_number_of_nodes_in_path="$1"
query_to_run="$2"

INPUT_FILE="queries/t0${query_to_run}-max-path-template.mql"
OUTPUT_FILE=$(mktemp --suffix=.mql)

echo "Running query on $INPUT_FILE with MAX_PATH=$max_number_of_nodes_in_path"
sed "s/MAX_PATH/$max_number_of_nodes_in_path/g" "$INPUT_FILE" > "$OUTPUT_FILE"


bash $MILLENIUM_DB_HOME_DIR/scripts/query $OUTPUT_FILE
