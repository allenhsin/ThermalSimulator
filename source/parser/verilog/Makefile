
language        := verilog

DIRS_TO_SOURCE  := . \
                   expressions \
                   raw

YACC            = bison
LEX             = flex

DIRS            = $(patsubst %,$(CURDIR)/%,$(DIRS_TO_SOURCE))
SRCS            = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
                    
OBJS            = $(patsubst %.cc,%.o, $(SRCS))

DEBUG_FLAGS     = -g
OPT_FLAGS       = -O2 -fPIC
WARN_FLAGS      = -Wall -W # -pedantic
CXXFLAGS        = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDE_FLAGS)

YACC_OUT        = yacc.$(language).cc
LEX_OUT         = lex.$(language).cc

YACC_OBJ        = $(patsubst %.cc,%.o, $(YACC_OUT))
LEX_OBJ         = $(patsubst %.cc,%.o, $(LEX_OUT))

TARGET          = $(CURDIR)/lib$(language).a

all: $(TARGET)

$(TARGET): $(LEX_OBJ) $(YACC_OBJ) $(OBJS)
	$(AR) -rcs $@ $^

# $(OBJS): $(SRCS)
	# $(CXX) $(CXXFLAGS) -c $< -o $@
    
# For general c++ compilation
%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@
    
$(YACC_OUT): $(language).y $(generated)
	$(YACC) -o yacc.$(language).cc -d $(language).y

$(LEX_OUT): $(language).l $(generated) $(YACC_OUT)
	$(LEX) -o $@ $(language).l

$(YACC_OBJ): $(YACC_OUT)
	$(CXX) $(CXXFLAGS) -c $< -o $@
    
$(LEX_OBJ): $(LEX_OUT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(LEX_OUT) $(YACC_OUT) $(LEX_OBJ) $(YACC_OBJ)
    
