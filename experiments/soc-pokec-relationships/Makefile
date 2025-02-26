SCRIPTS_DIR := $(abspath prepare_data/scripts)
BUILD_DIR := $(abspath prepare_data/build)
DATA_DIR := $(abspath prepare_data/data)
RAW_DATA_DIR := $(abspath prepare_data/raw_data)
MILLENIUMDB_BUILD_DIR := $(abspath ../../build/standard)

all: prepare_pokec_pathfinder

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

$(RAW_DATA_DIR)/soc-pokec-relationships.txt:
	mkdir -p $(RAW_DATA_DIR)
	wget -O $(RAW_DATA_DIR)/soc-pokec-relationships.txt.gz https://snap.stanford.edu/data/soc-pokec-relationships.txt.gz
	gzip -d $(RAW_DATA_DIR)/soc-pokec-relationships.txt.gz

prepare_pokec_pathfinder: $(RAW_DATA_DIR)/soc-pokec-relationships.txt $(BUILD_DIR) $(DATA_DIR)
	# @echo "Creating temporary directory..."
	cp $(SCRIPTS_DIR)/process_pokec.py $(SCRIPTS_DIR)/pokec_to_pathfinder.py $(BUILD_DIR)/
	cd $(BUILD_DIR) && python3 process_pokec.py $(RAW_DATA_DIR)/soc-pokec-relationships.txt
	cd $(BUILD_DIR) && python3 pokec_to_pathfinder.py pokec_edges.csv Knows
	cp $(BUILD_DIR)/pokec_pathfinder.txt $(DATA_DIR)/
	@echo "Database file created at $(DATA_DIR)/pokec_pathfinder.txt"

add_starting_node_labels_to_pokec_pathfinder: $(DATA_DIR)/pokec_pathfinder.txt $(BUILD_DIR)
	cp $(DATA_DIR)/pokec_pathfinder.txt $(BUILD_DIR)/pokec_pathfinder_tmp.txt
	python $(SCRIPTS_DIR)/add_starting_nodes.py > $(DATA_DIR)/starting_nodes_pokec_pathfinder.txt
	echo "" >> $(BUILD_DIR)/pokec_pathfinder_tmp.txt
	cat $(DATA_DIR)/starting_nodes_pokec_pathfinder.txt >> $(BUILD_DIR)/pokec_pathfinder_tmp.txt
	cp $(BUILD_DIR)/pokec_pathfinder_tmp.txt $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.txt
	@echo "Created file $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.txt sucessfully"

create_pokec_db: $(DATA_DIR)
	mkdir -p $(DATA_DIR)/databases
	$(MILLENIUMDB_BUILD_DIR)/bin/mdb-import $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.txt $(DATA_DIR)/databases/pokec_db)

clean:
	@echo "Cleaning up build directory..."
	rm -rf $(BUILD_DIR)
