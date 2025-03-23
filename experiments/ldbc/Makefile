SF ?= 0.1
SCRIPTS_DIR := $(abspath prepare_data/scripts)
BUILD_DIR := $(abspath prepare_data/build)
DATA_DIR := $(abspath data)
RAW_DATA_DIR := $(abspath prepare_data/raw_data)
MILLENIUMDB_BUILD_DIR := $(abspath ../../build/standard)

.PHONY: check_raw_files database_file_without_starting_nodes database_file_with_starting_nodes ldbc_db clean

all: ldbc_db

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
	tools/run.py --parallelism 4 --memory 6G -- --scale-factor $(SF) --output-dir $$tmpdir && \
	mkdir -p $(RAW_DATA_DIR)/sf-$(SF)/ && \
    find $$tmpdir/graphs/csv/raw/composite-merged-fk/dynamic/Person_knows_Person/ -type f -name "*.csv" -print0 |  xargs -0 awk 'FNR==1 && NR!=1 {next} {print}' > $(RAW_DATA_DIR)/sf-$(SF)/Person_knows_Person.csv
	rm -rf $$tmpdir

check_raw_files: $(RAW_DATA_DIR)/sf-$(SF)/Person_knows_Person.csv
	@echo "File exists, proceeding..."

$(DATA_DIR)/ldbc_$(SF).txt: check_raw_files
	mkdir -p $(DATA_DIR)
	mkdir -p $(BUILD_DIR)
	source venv/bin/activate && \
	python3 $(SCRIPTS_DIR)/convert_csv_to_txt.py $(RAW_DATA_DIR)/sf-$(SF)/Person_knows_Person.csv >$(DATA_DIR)/ldbc_$(SF).txt
	echo "Database file created at $(DATA_DIR)/ldbc_$(SF).txt"

database_file_without_starting_nodes: $(DATA_DIR)/ldbc_$(SF).txt


$(DATA_DIR)/ldbc_$(SF)_with_starting_nodes.qm: database_file_without_starting_nodes
	mkdir -p $(BUILD_DIR)
	cp $(DATA_DIR)/ldbc_$(SF).txt $(BUILD_DIR)/ldbc_$(SF)_tmp.txt
	source venv/bin/activate && \
	python3 $(SCRIPTS_DIR)/add_starting_nodes.py --file_with_graph $(BUILD_DIR)/ldbc_$(SF)_tmp.txt >$(DATA_DIR)/starting_nodes_ldbc_$(SF).txt
	echo "" >> $(BUILD_DIR)/ldbc_$(SF)_tmp.txt
	cat $(DATA_DIR)/starting_nodes_ldbc_$(SF).txt >> $(BUILD_DIR)/ldbc_$(SF)_tmp.txt
	cp $(BUILD_DIR)/ldbc_$(SF)_tmp.txt $(DATA_DIR)/ldbc_$(SF)_with_starting_nodes.qm
	@echo "Created file $(DATA_DIR)/ldbc_$(SF)_with_starting_nodes.qm sucessfully"

database_file_with_starting_nodes: $(DATA_DIR)/ldbc_$(SF)_with_starting_nodes.qm
	
	
$(DATA_DIR)/databases/ldbc_$(SF)_db: database_file_with_starting_nodes 
	mkdir -p $(DATA_DIR)/databases
	$(MILLENIUMDB_BUILD_DIR)/bin/mdb-import $(DATA_DIR)/ldbc_$(SF)_with_starting_nodes.qm  $(DATA_DIR)/databases/ldbc_$(SF)_db

ldbc_db: $(DATA_DIR)/databases/ldbc_$(SF)_db

# clean:
# 	@echo "Cleaning up build directory..."
# 	rm -rf $(BUILD_DIR)
# 	rm -rf $(DATA_DIR)/*
