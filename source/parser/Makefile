
DIRS_TO_SOURCE  := .

DIRS            = $(patsubst %,$(CURDIR)/%,$(DIRS_TO_SOURCE))
SRCS            = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
                    
OBJS            = $(patsubst %.cpp,%.opp, $(patsubst %.cc,%.o, $(patsubst %.c,%.o,$(SRCS))))

DEBUG_FLAGS     = -g
OPT_FLAGS       = -O2 -fPIC
WARN_FLAGS      = -Wall -W # -pedantic
CXXFLAGS        = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDE_FLAGS)

TARGET          = $(CURDIR)/test

LD_LIBS         += -lverilog -lsnazzle
LD_FLAGS        += -L$(CURDIR)/verilog -L$(CURDIR)/snazzle
LIB_VERILOG     = verilog/libverilog.a
LIB_SNAZZLE     = snazzle/libsnazzle.a

all: $(TARGET)

$(TARGET): $(LIB_VERILOG) $(LIB_SNAZZLE) $(OBJS) 
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) $(OBJS) -lfl $(LD_LIBS) -o $(TARGET)
    
# For general c++ compilation
%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB_VERILOG):
	make -C $(CURDIR)/verilog

$(LIB_SNAZZLE):
	make -C $(CURDIR)/snazzle
    
clean:
	make -C $(CURDIR)/verilog clean
	make -C $(CURDIR)/snazzle clean
	rm -rf $(TARGET) $(OBJS)
    
.PHONY: $(LIB_VERILOG) $(LIB_SNAZZLE)
    