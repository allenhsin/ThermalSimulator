# parameters

SIM_ROOT                ?= $(CURDIR)
OUTPUT_DIR              ?= $(shell date +%Y-%m-%d_%H-%M-%S)
CONFIG_FILE_DIR         ?= $(SIM_ROOT)/configs
CONFIG_FILE_NAME        ?= thermal_sim.cfg
CONFIG_FILE_ABS_PATH    := $(CONFIG_FILE_DIR)/$(CONFIG_FILE_NAME)
OUTPUT_DIR_ABS_PATH     := $(SIM_ROOT)/results/$(OUTPUT_DIR)


# Define the directories that will be compiled
DIRS_TO_SOURCE  := source \
                   source/data \
                   source/misc \
                   source/system \
                   source/models \
                   source/models/performance_model \
                   source/models/thermal_model \
                   source/models/physical_model \
                   source/models/physical_model/device_models \
                   include/libutil

DIRS = $(patsubst %,$(CURDIR)/%,$(DIRS_TO_SOURCE))
SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
OBJS = $(patsubst %.cpp,%.opp,$(patsubst %.cc,%.o,$(SRCS)))

INCLUDE_FLAGS   = -I$(CURDIR) -I$(CURDIR)/include
DEBUG_FLAGS     = -g
OPT_FLAGS       = -O2 -fPIC
WARN_FLAGS      = -Wall -W # -pedantic
CXXFLAGS        = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDE_FLAGS)

LD_LIBS         += -lverilog
LD_FLAGS        += -L$(CURDIR)/source/parser/verilog -L$(CURDIR)/lib -Wl,-rpath,$(CURDIR)/lib

LIB_VERILOG     = source/parser/verilog/libverilog.a

TARGET          = $(CURDIR)/thermal_sim
COMMAND         := $(TARGET) --config=$(CONFIG_FILE_ABS_PATH) --general/result_dir=$(OUTPUT_DIR_ABS_PATH)

all: $(TARGET)

$(LIB_VERILOG):
	make -C source/parser/verilog

$(TARGET): $(OBJS) $(LIB_VERILOG)
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) $(OBJS) -o $(TARGET) $(LD_LIBS) 

# For general c++ compilation
%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.opp: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.oxx: %.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

%/created:
	mkdir -p $(dir $@)
	touch $@

.phony: clean clean_results run create_output_dir

run: create_output_dir
	$(COMMAND)

create_output_dir:
	mkdir -p $(OUTPUT_DIR_ABS_PATH)
	echo "$(COMMAND)" > $(OUTPUT_DIR_ABS_PATH)/command
	rm -f $(SIM_ROOT)/results/latest
	ln -s $(OUTPUT_DIR_ABS_PATH) $(SIM_ROOT)/results/latest

clean:
	make clean_results
	make -C source/parser/verilog clean
	$(RM) -rf $(OBJS) $(TARGET)

clean_results:
	rm -f $(SIM_ROOT)/results/latest
	rm -rf $(SIM_ROOT)/results/[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]_[0-9][0-9]-[0-9][0-9]-[0-9][0-9]

