SF ?= 0.1
SCRIPTS_DIR := $(abspath prepare_data/scripts)
BUILD_DIR := $(abspath prepare_data/build)
DATA_DIR := $(abspath data)
RAW_DATA_DIR := $(abspath prepare_data/raw_data)
MILLENIUMDB_BUILD_DIR := $(abspath ../../build/standard)

.PHONY: check_raw_files make_database_file_without_starting_nodes clean

$(RAW_DATA_DIR)/sf-$(SF)/Person_knows_Person.csv:
	@set -x; tmpdir=$$(mktemp -d) && \
	echo "Temporary directory: $$tmpdir" && \
	cd ~/ldbc_snb_datagen_spark && \
	PLATFORM_VERSION=$$(sbt -batch -error 'print platformVersion') && export PLATFORM_VERSION && \
	DATAGEN_VERSION=$$(sbt -batch -error 'print version') && export DATAGEN_VERSION && \
	LDBC_SNB_DATAGEN_JAR=$$(sbt -batch -error 'print assembly / assemblyOutputPath') && export LDBC_SNB_DATAGEN_JAR && \
	echo "LDBC JAR: $$LDBC_SNB_DATAGEN_JAR" && \
	test -f venv/bin/activate || { echo "venv not found!"; exit 1; } && \
	source venv/bin/activate && \
	tools/run.py -- --scale-factor $(SF) --output-dir $$tmpdir && \
	mkdir -p $(RAW_DATA_DIR)/sf-$(SF)/ && \
	cp $$(find $$tmpdir/graphs/csv/raw/composite-merged-fk/dynamic/Person_knows_Person/ -type f -name "*.csv") $(RAW_DATA_DIR)/sf-$(SF)/Person_knows_Person.csv
#	rm -rf $$tmpdir

check_raw_files: $(RAW_DATA_DIR)/sf-$(SF)/Person_knows_Person.csv
	@echo "File exists, proceeding..."

$(DATA_DIR)/ldbc_$(SF).txt: check_raw_files
	mkdir -p $(DATA_DIR)
	mkdir -p $(BUILD_DIR)
	python3 $(SCRIPTS_DIR)/convert_csv_to_txt.py $(RAW_DATA_DIR)/sf-$(SF)/Person_knows_Person.csv >$(DATA_DIR)/ldbc_$(SF).txt
	echo "Database file created at $(DATA_DIR)/ldbc_$(SF).txt"

make_database_file_without_starting_nodes: $(DATA_DIR)/ldbc_$(SF).txt

# $(DATA_DIR)/ldbc_$(SF)_with_starting_nodes.qm: make_database_file_without_starting_nodes


# $(DATA_DIR)/pokec_pathfinder.txt: check_raw_files 
# 	mkdir -p $(DATA_DIR)
# 	mkdir -p $(BUILD_DIR)
# 	cp $(SCRIPTS_DIR)/process_pokec.py $(SCRIPTS_DIR)/pokec_to_pathfinder.py $(BUILD_DIR)/
# 	cd $(BUILD_DIR) && python3 process_pokec.py $(RAW_DATA_DIR)/soc-pokec-relationships.txt
# 	cd $(BUILD_DIR) && python3 pokec_to_pathfinder.py pokec_edges.csv Knows
# 	cp $(BUILD_DIR)/pokec_pathfinder.txt $(DATA_DIR)/
# 	@echo "Database file created at $(DATA_DIR)/pokec_pathfinder.txt"
# 
# pokec_with_starting_nodes: $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.qm 
# 
# $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.qm: $(DATA_DIR)/pokec_pathfinder.txt
# 	mkdir -p $(BUILD_DIR)
# 	cp $(DATA_DIR)/pokec_pathfinder.txt $(BUILD_DIR)/pokec_pathfinder_tmp.txt
# 	python3 $(SCRIPTS_DIR)/add_starting_nodes.py > $(DATA_DIR)/starting_nodes_pokec_pathfinder.txt
# 	echo "" >> $(BUILD_DIR)/pokec_pathfinder_tmp.txt
# 	cat $(DATA_DIR)/starting_nodes_pokec_pathfinder.txt >> $(BUILD_DIR)/pokec_pathfinder_tmp.txt
# 	cp $(BUILD_DIR)/pokec_pathfinder_tmp.txt $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.qm
# 	@echo "Created file $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.qm sucessfully"
# 
# pokec_db: $(DATA_DIR)/databases/pokec_db 
# 
# $(DATA_DIR)/databases/pokec_db: $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.qm 
# 	mkdir -p $(DATA_DIR)/databases
# 	$(MILLENIUMDB_BUILD_DIR)/bin/mdb-import $(DATA_DIR)/pokec_pathfinder_with_starting_nodes.qm  $(DATA_DIR)/databases/pokec_db

clean:
	@echo "Cleaning up build directory..."
	rm -rf $(BUILD_DIR)
	rm -rf $(DATA_DIR)/*
