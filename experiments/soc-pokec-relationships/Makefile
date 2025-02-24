SCRIPTS_DIR := $(abspath prepare_data/scripts)
BUILD_DIR := $(abspath prepare_data/build)
DATA_DIR := $(abspath prepare_data/data)
RAW_DATA_DIR := $(abspath prepare_data/raw_data)

all: prepare_pokec_pathfinder

$(RAW_DATA_DIR)/soc-pokec-relationships.txt:
	mkdir -p $(RAW_DATA_DIR)
	wget -O $(RAW_DATA_DIR)/soc-pokec-relationships.txt.gz https://snap.stanford.edu/data/soc-pokec-relationships.txt.gz
	gzip -d $(RAW_DATA_DIR)/soc-pokec-relationships.txt.gz

prepare_pokec_pathfinder: $(RAW_DATA_DIR)/soc-pokec-relationships.txt
	# @echo "Creating temporary directory..."
	mkdir -p $(BUILD_DIR)
	mkdir -p $(DATA_DIR)
	cp $(SCRIPTS_DIR)/process_pokec.py $(SCRIPTS_DIR)/pokec_to_pathfinder.py $(BUILD_DIR)/
	cd $(BUILD_DIR) && python3 process_pokec.py $(RAW_DATA_DIR)/soc-pokec-relationships.txt
	cd $(BUILD_DIR) && python3 pokec_to_pathfinder.py pokec_edges.csv Knows
	cp $(BUILD_DIR)/pokec_pathfinder.txt $(DATA_DIR)/
	@echo "Database file created at $(DATA_DIR)/pokec_pathfinder.txt"

clean:
	@echo "Cleaning up build directory..."
	rm -rf $(BUILD_DIR)
