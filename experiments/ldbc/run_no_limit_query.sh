#!/bin/bash

MILLENIUM_DB_HOME_DIR="/home/mateusz/studia-current/mgr/MillenniumDB"

# Check if the correct number of arguments is provided
if [ "$#" -lt 0 ]; then
    echo "Usage: $0"
    exit 1
fi

QUERY_FILE="queries/t01.mql"


bash $MILLENIUM_DB_HOME_DIR/scripts/query $QUERY_FILE
