#!/bin/bash

MILLENIUM_DB_HOME_DIR="/home/mateusz/studia-current/mgr/MillenniumDB"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <max_number_of_nodes_in_path>"
    exit 1
fi

max_number_of_nodes_in_path="$1"

INPUT_FILE="queries/t01-max-path-template.mql"
OUTPUT_FILE=$(mktemp --suffix=.mql)

sed "s/MAX_PATH/$max_number_of_nodes_in_path/g" "$INPUT_FILE" > "$OUTPUT_FILE"

cat $OUTPUT_FILE

bash $MILLENIUM_DB_HOME_DIR/scripts/query $OUTPUT_FILE
