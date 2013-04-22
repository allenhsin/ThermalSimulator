
parser          := verilog

generated       := $(CURDIR)/generated

DIRS_TO_SOURCE  := .

YACC            = bison
LEX             = flex

DIRS            = $(patsubst %,$(CURDIR)/%,$(DIRS_TO_SOURCE))
SRCS            = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc)) \
                    $(LEX_OUT) $(YACC_OUT)
                    
OBJS            = $(patsubst %.cpp,%.opp, $(patsubst %.cc,%.o, $(patsubst %.c,%.o,$(SRCS))))

DEBUG_FLAGS     = -g
OPT_FLAGS       = -O2 -fPIC
WARN_FLAGS      = -Wall -W # -pedantic
CXXFLAGS        = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDE_FLAGS)

TARGET          = $(CURDIR)/snazzle

YACC_OUT        = $(generated)/yacc.$(parser).cc $(generated)/yacc.$(parser).hh
LEX_OUT         = $(generated)/lex.$(parser).cc

all: $(TARGET)

$(TARGET): $(YACC_OUT) $(LEX_OUT) $(OBJS) 
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) $(OBJS) -lfl -o $(TARGET)
    
# For general c++ compilation
%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
    
$(YACC_OUT): snazzle.y $(generated)
	$(YACC) -o $(generated)/yacc.$(parser).cc -d snazzle.y

$(LEX_OUT): snazzle.l $(generated) $(YACC_OUT)
	$(LEX) -o $@ snazzle.l
    
$(generated):
	mkdir -p $(generated)
    
clean:
	rm -rf $(TARGET) $(OBJS) $(LEX_OUT) $(YACC_OUT)
    
