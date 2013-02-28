
# Define the directories that will be compiled
DIRS_TO_SOURCE  := source \
                   source/data \
                   source/system \
                   source/models
DIRS_TO_CONFIG  := source/system/config

DIRS = $(patsubst %,$(CURDIR)/%,$(DIRS_TO_SOURCE))
SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc)) \
       $(wildcard $(CURDIR)/$(DIRS_TO_CONFIG)/*.cpp)
OBJS = $(patsubst %.cpp,%.opp,$(patsubst %.cc,%.o,$(SRCS)))

INCLUDE_FLAGS = -I$(CURDIR)/include -I$(CURDIR)/$(DIRS_TO_CONFIG)
DEBUG_FLAGS   = -g
OPT_FLAGS     = -O2 -fPIC
WARN_FLAGS    = -pedantic -Wall -W
CXXFLAGS      = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDE_FLAGS)

LD_LIBS  += -lboost_filesystem -lboost_system -lboost_thread -lboost_regex
LD_FLAGS += -L$(CURDIR)/libs

TARGET      = $(CURDIR)/thermal_sim
CONFIG_TEST = $(CURDIR)/config_test

all: $(TARGET)
config_test: $(CONFIG_TEST)

$(TARGET): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) $(OBJS) -o $(TARGET) $(LD_LIBS) 

$(CONFIG_TEST): $(OBJS) $(CURDIR)/$(DIRS_TO_CONFIG)/test/test.oxx 
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) $(OBJS) $(CURDIR)/$(DIRS_TO_CONFIG)/test/test.oxx -o $(CONFIG_TEST) $(LD_LIBS)

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

.phony: clean
clean:
	$(RM) -rf $(OBJS) $(TARGET) $(CONFIG_TEST)

