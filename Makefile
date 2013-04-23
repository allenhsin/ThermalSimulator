
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
DIRS_TO_CONFIG  := source/misc/config

DIRS = $(patsubst %,$(CURDIR)/%,$(DIRS_TO_SOURCE))
SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc)) \
       $(wildcard $(CURDIR)/$(DIRS_TO_CONFIG)/*.cpp)
OBJS = $(patsubst %.cpp,%.opp,$(patsubst %.cc,%.o,$(SRCS)))

INCLUDE_FLAGS = -I$(CURDIR) -I$(CURDIR)/include -I$(CURDIR)/$(DIRS_TO_CONFIG)
DEBUG_FLAGS   = -g
OPT_FLAGS     = -O2 -fPIC
WARN_FLAGS    = -Wall -W # -pedantic
CXXFLAGS      = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDE_FLAGS)

LD_LIBS  += -lboost_filesystem -lboost_system
LD_FLAGS += -L$(CURDIR)/lib -Wl,-rpath,$(CURDIR)/lib

TARGET      = $(CURDIR)/thermal_sim

all: $(TARGET)

$(TARGET): $(OBJS) 
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

.phony: clean clean_results run

run:
	make clean_results
	$(TARGET) &

clean:
	make clean_results
	$(RM) -rf $(OBJS) $(TARGET)

clean_results:
	$(RM) -rf results/*.out results/*.log results/*.dbg

