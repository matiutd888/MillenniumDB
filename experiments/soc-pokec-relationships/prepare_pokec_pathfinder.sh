wget https://snap.stanford.edu/data/soc-pokec-relationships.txt.gz
gzip -d soc-pokec-relationships.txt.gz
python3 process_pokec.py soc-pokec-relationships.txt
python3 pokec_to_pathfinder.py pokec_edges.csv Knows
