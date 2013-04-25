
language        := verilog

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

YACC_OUT        = yacc.$(language).cc yacc.$(language).hh
LEX_OUT         = lex.$(language).cc

TARGET          = $(CURDIR)/lib$(language).a

all: $(TARGET)

$(TARGET): $(YACC_OUT) $(YACC_OUT) $(OBJS)
	$(AR) -rcs $@ $^

# For general c++ compilation
%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
    
$(YACC_OUT): $(language).y $(generated)
	$(YACC) -o yacc.$(language).cc -d $(language).y

$(LEX_OUT): $(language).l $(generated) $(YACC_OUT)
	$(LEX) -o $@ $(language).l
    
clean:
	rm -rf $(TARGET) $(OBJS) $(LEX_OUT) $(YACC_OUT)
    
