
/**
 *
 * This is simple verilog netlist parser which will cover ONLY the structural
 * netlist language subset of verilog. The only behavioral constructs the
 * parser recognizes are used for supporting parametrized netlists. Syntax
 * checking will not be rigorous.
 *
 * WARNING: This code is very likely to MEMORY-LEAK!!! Use at your own risk.
 *
 */

// This will be a verilog parser, so prefix everything with verilog
%define api.prefix "verilog"

// Let yyparse take a verilog file pointer as an input
%parse-param { VerilogParser::VerilogFileReader* reader_ }

%error-verbose

%{
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>

#include "VerilogFileReader.h"
#include "VerilogFile.h"

#include "VerilogModule.h"
#include "VerilogItem.h"
#include "VerilogMisc.h"

#include "VerilogNumber.h"
#include "VerilogParameter.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"

#include "VerilogScope.h"

#include "expressions/StringExpression.h"

#include <cstdlib>

using namespace std;
using namespace VerilogParser;

#define yyin verilogin

// stuff from flex that bison needs to know about:
int yylex();

extern int line_num;

extern FILE* yyin;
void yyerror(VerilogFileReader* file_, const char *s);
%}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	char *sval;
    char cval;
    
    VerilogParser::VerilogBinOp bop;
    VerilogParser::VerilogNumber* num;    
    VerilogParser::VerilogExpression* expr;
    VerilogParser::VerilogRange* range;
    VerilogParser::VerilogVariables* variables;
    VerilogParser::VerilogConn* conn;
    
    VerilogParser::VerilogPortType port_type;
    VerilogParser::VerilogNetType net_type;
    
    VerilogParser::VerilogExpressions* exprs;
    VerilogParser::VerilogConns* conns;
    
    VerilogParser::VerilogModule* module;
    VerilogParser::VerilogModules* modules;

    VerilogParser::VerilogItem* item;
    VerilogParser::VerilogItems* items;

    VerilogParser::VerilogNet* net;
    VerilogParser::VerilogNets* nets;
    VerilogParser::VerilogInstance* instance;
    VerilogParser::VerilogInstances* instances;
    VerilogParser::VerilogParameter* parameter;
    VerilogParser::VerilogParameters* parameters;
    
}

// define the constant-string tokens:
%token  ASSIGN          "assign"
%token  DEFPARAM        "defparam"
%token  ENDGENERATE     "endgenerate"
%token  ENDMODULE       "endmodule"
%token  GENERATE        "generate"
%token  GENVAR          "genvar"
%token  INOUT           "inout"
%token  INPUT           "input"
%token  MODULE          "module"
%token  OUTPUT          "output"
%token  PARAMETER       "parameter"
%token  WIRE            "wire"

%token  <sval>          '+'
%token  <sval>          '-'
%token  <sval>          '*'
%token  <sval>          '/'
%token  <sval>          '%'

// define some terminal symbol data types
%token  <fval>          FLOAT
%token  <sval>          IDENTIFIER
%token  <num>           NUMBER
%token  <sval>          STRING

// Define non-terminal symbol data types
// %type   <bop>           binary_operator
%type   <expr>          expression
%type   <expr>          constant_expression
%type   <exprs>         concatenation
%type   <exprs>         list_of_expressions
%type   <variables>     list_of_variables

%type   <variables>     list_of_ports

%type   <port_type>     port_dir
%type   <net_type>      net_type
%type   <range>         range

%type   <module>        description
%type   <module>        module_declaration
%type   <modules>       source_text

%type   <items>         module_item
%type   <items>         module_item_list

%type   <items>         net_declaration

%type   <parameter>     param_assignment
%type   <items>         list_of_param_assignments
%type   <items>         parameter_declaration

%type   <items>         module_instantiation
%type   <instances>     list_of_module_instances

%type   <conn>          named_connection
%type   <conns>         list_of_named_connections
%type   <conns>         list_of_named_port_connections
%type   <conns>         list_of_named_parameter_value_assignment
%type   <conns>         parameter_value_assignment
%type   <conns>         list_of_module_port_connections
%type   <conns>         list_of_module_connections

%left '+' '-'
%left '*' '/' '%'

%start source_text

%%

source_text: 
    /* empty */                                     { ; }
    | source_text description                       { reader_->addModule($2); }
    ;

module_declaration:
    MODULE IDENTIFIER '(' list_of_ports ')' ';' module_item_list ENDMODULE
        { $$ = new VerilogModule($2, *$7); delete $7; }
    ;
        
// The list of ports is online used to describe the port ordering if this module is instantiated
// using ordered (unnamed) port connections
list_of_ports:
    /* empty */                                     { $$ = new VerilogVariables(); }
    | IDENTIFIER                                    { $$ = new VerilogVariables(1, $1); }
    | list_of_ports ',' IDENTIFIER                  { $$ = $1; $1->push_back($3); }
    ;
    
module_item_list:
    /* empty */                                     { $$ = new VerilogItems(); }
    | module_item_list module_item                  { $$ = $1; $$->insert($$->end(), $2->begin(), $2->end()); delete $2; }
    ;

net_declaration:
    net_type list_of_variables ';'                  { $$ = VerilogNet::createVerilogNets(*$2, PORT_NONE, $1); delete $2; }
    | net_type range list_of_variables ';'          { $$ = VerilogNet::createVerilogNets(*$3, PORT_NONE, $1, *$2); delete $2; delete $3; }
    | port_dir list_of_variables ';'                { $$ = VerilogNet::createVerilogNets(*$2, $1, NET_WIRE); delete $2; }
    | port_dir range list_of_variables ';'          { $$ = VerilogNet::createVerilogNets(*$3, $1, NET_WIRE, *$2); delete $2; delete $3; }
    | port_dir net_type list_of_variables ';'       { $$ = VerilogNet::createVerilogNets(*$3, $1, $2); delete $3; }
    | port_dir net_type range list_of_variables ';' { $$ = VerilogNet::createVerilogNets(*$4, $1, $2, *$3); delete $3; delete $4; }
    ;    
    
range:
    '[' constant_expression ']'                             { $$ = new VerilogRange(*$2, *$2); delete $2; }
    | '[' constant_expression ':' constant_expression ']'   { $$ = new VerilogRange(*$2, *$4); delete $2; delete $4; }
    ;

port_dir:
    INPUT           { $$ = PORT_INPUT; }
    | OUTPUT        { $$ = PORT_OUTPUT; }
    | INOUT         { $$ = PORT_INOUT; }
    ;

net_type:
    WIRE            { $$ = NET_WIRE; }
        
parameter_declaration:
    PARAMETER list_of_param_assignments ';'             { $$ = $2; }
    ;

list_of_param_assignments:
    /* empty */                                         { $$ = new VerilogItems(); }
    | param_assignment                                  { $$ = new VerilogItems(1, $1); }
    | list_of_param_assignments ',' param_assignment    { $$ = $1; $$->push_back($3); }
    ;
        
param_assignment:
    IDENTIFIER '=' constant_expression                  { $$ = new VerilogParameter($1, *$3); delete $3; }
    ;

module_instantiation:
    IDENTIFIER parameter_value_assignment list_of_module_instances ';' 
        { 
            // I don't like how this is written, probably should rewrite the data
            // structure a bit to minimize the chance of accidental memory leaks
            VerilogInstances::const_iterator it;
            for (it = $3->begin(); it != $3->end(); ++it)
            {
                // Make a deep copy of $2 for every instance            
                VerilogConns* new_conns = new VerilogConns();
                VerilogConns::const_iterator c_it;
                for (c_it = $2->begin(); c_it != $2->end(); ++c_it)
                    new_conns->push_back(new VerilogConn(*(*c_it)));
                
                (*it)->setModuleName($1);
                (*it)->setParameterValues(*new_conns);
                
                delete new_conns;
            }

            $$ = new VerilogItems($3->begin(), $3->end());
            
            deletePtrVector<VerilogConn>($2);
            delete $3;
        }
    | IDENTIFIER list_of_module_instances ';' 
        { 
            VerilogInstances::const_iterator it;
            for (it = $2->begin(); it != $2->end(); ++it)
                (*it)->setModuleName($1);

            $$ = new VerilogItems($2->begin(), $2->end());
            delete $2;
        }
    ;

parameter_value_assignment:
    '#' '(' list_of_named_parameter_value_assignment ')'    { $$ = $3; }
    
list_of_named_parameter_value_assignment:
    /* empty */                                             { $$ = new VerilogConns(); }
    | list_of_named_connections                             { $$ = $1; }
    ;
    
// A list of named connections following the .IDENTIFIER(EXPRESSION) format
// Used for both port connections and parameter assignments
list_of_named_connections:
    named_connection                                        { $$ = new VerilogConns(); $$->push_back($1); }
    | list_of_named_connections ',' named_connection        { $$ = $1; $$->push_back($3); }
    ;
    
list_of_module_instances:
    IDENTIFIER '(' list_of_module_connections ')'                                 { $$ = new VerilogInstances(1, new VerilogInstance($1, *$3)); delete $3; }
    | list_of_module_instances IDENTIFIER '(' list_of_module_connections ')'      { $$ = $1; $$->push_back(new VerilogInstance($2, *$4)); delete $4; }
    ;
    
// Must used named ports for the whole thing or implicit ports, cannot mix between the two
list_of_module_connections:
    /* empty */                                             { $$ = new VerilogConns(); }
    | list_of_module_port_connections                       { $$ = $1; }
    | list_of_named_port_connections                        { $$ = $1; }
    ;
    
list_of_module_port_connections:
    list_of_expressions
        { 
            // Convert a list of expressions to port connections
            $$ = new VerilogConns();
            // Just insert empty string as the port name and the verilog module should understand
            VerilogExpressions::const_iterator it;
            for (it = $1->begin(); it != $1->end(); ++it)
            {
                $$->push_back(new VerilogConn("", *(*it)));
                delete *it;
            }
            delete $1;
        }
    ;

list_of_named_port_connections:
    list_of_named_connections
    ;
    
named_connection:
    '.' IDENTIFIER '(' expression ')'       { $$ = new VerilogConn($2, *$4); delete $4; }
    ;
    
// TODO stack

// For now, I don't know what else is important in a verilog netlist file for us besides
// just module declarations
description:
    module_declaration
    ;

// binary_operator:
    // '+'                                     { $$ = BOP_PLUS; }
    // | '-'                                   { $$ = BOP_MINUS; }
    // | '*'                                   { $$ = BOP_TIMES; }
    // | '/'                                   { $$ = BOP_DIV; }
    // | '%'                                   { $$ = BOP_MOD; }
    // ;
    
// Support very simple form of expressions, no unary/binary/conditional operations are
// supported. Thus, this is merged with "primary". STRINGS are also disallowed
expression:
    '(' expression ')'                      { $$ = $2; }
    | NUMBER                                { $$ = new VerilogExpression(*$1); delete $1; }
    | STRING                                { $$ = NULL; cout << (new StringExpression($1))->toString() << endl; }
    | IDENTIFIER                            { $$ = new VerilogExpression($1); }
    | IDENTIFIER range                      { $$ = new VerilogExpression($1, *$2); delete $2; }
    | concatenation                         { $$ = new VerilogExpression(*$1); delete $1; }
    | expression '+' expression             { $$ = new VerilogExpression(*$1, BOP_PLUS, *$3); delete $1; delete $3; }
    | expression '-' expression             { $$ = new VerilogExpression(*$1, BOP_MINUS, *$3); delete $1; delete $3; }
    | expression '*' expression             { $$ = new VerilogExpression(*$1, BOP_TIMES, *$3); delete $1; delete $3; }
    | expression '/' expression             { $$ = new VerilogExpression(*$1, BOP_DIV, *$3); delete $1; delete $3; }
    | expression '%' expression             { $$ = new VerilogExpression(*$1, BOP_MOD, *$3); delete $1; delete $3; }
    ;
    
constant_expression:
    expression                              { $$ = $1; }
    ;

list_of_expressions:
    expression                              { $$ = new VerilogExpressions(1, $1); }
    | list_of_expressions ',' expression    { $$ = $1; $$->push_back($3); }
    ;
    
list_of_variables:
    /* empty */                             { $$ = new VerilogVariables(); }
    | IDENTIFIER                            { $$ = new VerilogVariables(1, $1); }
    | list_of_variables ',' IDENTIFIER      { $$ = $1; $$->push_back($3); }
    ;

module_item:
    net_declaration
    | parameter_declaration
    | module_instantiation
    ;
    
concatenation:
    '{' list_of_expressions '}'             { $$ = $2; }
    ;
    
// continuous_assign:
    // ASSIGN list_of_assignments ';'

// list_of_assignments:
    // assignment
    // ;
    
// assignment:
    // IDENTIFIER '=' constant_expression    
        
// port:
    // port_expression
    // | '.' IDENTIFIER '(' port_expression ')'
    // | '.' IDENTIFIER '(' ')'
    // ;
    
// port_expression:
    // port_reference
    // | '{' port_reference_list '}'

// port_reference:
    // IDENTIFIER
    // | IDENTIFIER '[' constant_expression ']'
    // | IDENTIFIER '[' constant_expression ':' constant_expression ']'

// port_reference_list:
    // port_reference
    // | port_reference_list ',' port_reference
    // ;
        
    
// | module_instantiation
    
%%

namespace VerilogParser
{
    VerilogFileReader::VerilogFileReader()
    {
        m_modules_ = new vector<VerilogModule*>();
        m_scope_ = new VerilogScope();        
    }
    
    VerilogFileReader::~VerilogFileReader()
    {
        delete m_scope_;
        deletePtrVector<VerilogModule>(m_modules_);
        
        m_modules_ = NULL;
        m_scope_ = NULL;
    }

    void VerilogFileReader::elaborate()
    {
        // Elaborate all potential top-level modules
        VerilogModules::const_iterator it;
        for (it = m_modules_->begin(); it != m_modules_->end(); ++it)
        {
            // Put the module in the scope's raw modules
            m_scope_->addRawModule(*it);
            // (*it)->elaborate(m_scope_);
            // Make a copy of the module, with no parameter overrides and elaborate
            (new VerilogModule(*it))->elaborate(m_scope_);
        }
    }    
    
    void VerilogFileReader::addModule(VerilogModule* module_)
    {
        m_modules_->push_back(module_);
    }
    
    
    bool VerilogFileReader::parse(VerilogFile* file_)
    {
        m_cur_file_ = file_;
        if(file_ != NULL)
        {
            yyin = fopen(file_->getFileName().c_str(), "r");
            if( yyin == NULL ) 
            {
                cout << "VerilogFileReader::parse : [Error] Problem opening the input file: " << file_->getFileName() << "." << endl;
                throw -1;
            }
        } 
        else 
        {
            cout << "VerilogFileReader::parse : [Warning] File name not given for verilog file reader constructor, running from stdin" << endl;
        }
        return yyparse( this ) == 0;
    }
    
    VerilogItems* VerilogFileReader::getFlattenedItems(const string& top_module_name, 
        const std::string& hier_sep_) const
    {
        VerilogItems* items = new VerilogItems();
        addFlattenedItems(items, hier_sep_, m_scope_->getElabModule(top_module_name), "");
        
        VerilogEquivNetMap* net_map = mapEquivNets(items);        
        return items;
    }
    
    void VerilogFileReader::addFlattenedItems(VerilogItems* items_, const std::string& hier_sep_, 
        const VerilogModule* module_, const std::string& hier_name_)
    {
        // Get all things in the module
        VerilogItems::const_iterator it;
        for (it = module_->getItems()->begin(); it != module_->getItems()->end(); ++it)
        {
            VerilogItem* item = *it;
            if (item->getType() == ITEM_NET)
                items_->push_back(item->getFlattenedClone(hier_name_, hier_sep_));
            else if (item->getType() == ITEM_PARAMETER)
                items_->push_back(item->getFlattenedClone(hier_name_, hier_sep_));
            else if (item->getType() == ITEM_INSTANCE)
            {
                const VerilogInstance* inst = (const VerilogInstance*) item;                
                addFlattenedItems(items_, hier_sep_, inst->getModule(),
                    hier_name_ + inst->getIdentifier() + hier_sep_);         
                    
                items_->push_back(inst->getFlattenedClone(hier_name_, hier_sep_));
            }
        }
    }
    
    VerilogEquivNetMap* VerilogFileReader::mapEquivNets(const VerilogItems* items_)
    {
        cout << "==================" << endl;
        VerilogEquivNetMap* equiv_map = new VerilogEquivNetMap();        
        for (VerilogItems::const_iterator it = items_->begin(); it != items_->end(); ++it)
        {
            // Add all nets to the map
            VerilogItem* item = *it;
            if (item->getType() == ITEM_NET)
            {
                const VerilogNet* net = (const VerilogNet*) item;
                for (unsigned int i = net->getRange().second.getNumber().getUInt();
                    i <= net->getRange().first.getNumber().getUInt(); ++i)
                {
                    ostringstream name;
                    name << net->getIdentifier() << "[" << i << "]";
                    (*equiv_map)[name.str()] = "";  
                    cout << name.str() << endl;
                }
            }
            // Right now, instance port connections are the only things that can
            // connect nets together to be equivalent
            else if (item->getType() == ITEM_INSTANCE)
            {
                const VerilogInstance* inst = (const VerilogInstance*) item;
                addEquivNets(equiv_map, inst);
            }        
        }
        
        // Too lazy to write something efficient for this, this will have n^2 performance in the
        // worst case, which is really really bad...
        bool change = false;        
        do
        {
            change = false;
            for (VerilogEquivNetMap::const_iterator it = equiv_map->begin(); it != equiv_map->end(); ++it)
            {
                string cur_name = it->second;
                while (!(*equiv_map)[cur_name].empty())
                {
                    change = true;
                    cur_name = (*equiv_map)[cur_name];
                    
                    (*equiv_map)[it->first] = cur_name;
                    
                    cout << it->first << " = " << cur_name << " " << (*equiv_map)[cur_name] << endl;
                }
            }
            
        } while (change);
        
        cout << "==================" << endl;
        return equiv_map;
    }
    
    // Looks at an instance and adds the equivalent nets
    void VerilogFileReader::addEquivNets(VerilogEquivNetMap* equiv_map_, const VerilogInstance* inst_)
    {
        // For all the connections in the instance
        for (VerilogConns::const_iterator c_it = inst_->getConns()->begin();
            c_it != inst_->getConns()->end(); ++c_it)
        {
            const VerilogExpression& net = (*c_it)->second;
            const string& port = (*c_it)->first;
            
            // This is ridiculously unclean way of doing this, really need to rethink the data structure
            const string port_i = port.substr(port.find_last_of('.') + 1);            
            const VerilogRange& port_range = ((const VerilogNet*) inst_->getModule()->getItem(port_i))->getRange();            
            
            // TODO: For now make sure the net has the IDENTIFIER_RANGE format
            if (!net.getType() == VerilogExpression::IDENTIFIER_RANGE)
                throw VerilogException("Bad port connect");            

            unsigned int high = net.getRange().first.getNumber().getUInt();
            unsigned int low = net.getRange().second.getNumber().getUInt();
            // For every bit of the connection            
            unsigned int port_idx = port_range.second.getNumber().getUInt();            
            for (unsigned int i = low; i <= high; ++i)
            {
                // Get the full name of the current bit of the net
                ostringstream net_name;
                net_name << net.getIdentifier() << "[" << i << "]";
                
                ostringstream port_name;
                port_name << port << "[" << port_idx << "]";
                (*equiv_map_)[port_name.str()] = net_name.str();
                
                cout << port_name.str() << " = " << net_name.str() << endl;
                
                port_idx++;                
            }
        }
    }
    
}

// Arguments must match those to yyparse
void yyerror(VerilogFileReader* reader_, const char *s) {
	cout << "[Error] " << reader_->getCurFile()->getFileName() << ":" << line_num << " -> " << s << endl;
	// might as well halt now:
	exit(-1);
}