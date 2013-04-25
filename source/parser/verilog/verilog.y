
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
%parse-param { Thermal::VerilogFile* file_ }

%error-verbose

%{
#include <cstdio>
#include <iostream>
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

#include <cstdlib>

using namespace std;
using namespace Thermal;

#define yyin verilogin

// stuff from flex that bison needs to know about:
int yylex();

extern int line_num;

extern FILE* yyin;
void yyerror(VerilogFile* file_, const char *s);
%}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	char *sval;
    char cval;
    
    Thermal::VerilogNumber* num;    
    Thermal::VerilogExpression* expr;
    Thermal::VerilogRange* range;
    Thermal::VerilogVariables* variables;
    Thermal::VerilogConn* conn;
    
    Thermal::VerilogPortType port_type;
    Thermal::VerilogNetType net_type;
    
    Thermal::VerilogExpressions* exprs;
    Thermal::VerilogConns* conns;
    
    Thermal::VerilogModule* module;
    Thermal::VerilogModules* modules;

    Thermal::VerilogItem* item;
    Thermal::VerilogItems* items;

    Thermal::VerilogNet* net;
    Thermal::VerilogNets* nets;
    Thermal::VerilogInstance* instance;
    Thermal::VerilogInstances* instances;
    Thermal::VerilogParameter* parameter;
    Thermal::VerilogParameters* parameters;
    
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

// define some terminal symbol data types
%token  <fval>          FLOAT
%token  <sval>          IDENTIFIER
%token  <num>           NUMBER

// Define non-terminal symbol data types
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

%start source_text

%%

source_text: 
    /* empty */                                     { $$ = new VerilogModules(); }
    | source_text description                       { $$ = $1; $$->push_back($2); cout << $2->getName() << endl; }
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
            VerilogInstances::const_iterator it;
            for (it = $3->begin(); it != $3->end(); ++it)
            {
                (*it)->setModuleName($1);
                (*it)->setParameterValues(*$2);
            }

            $$ = new VerilogItems($3->begin(), $3->end());
            delete $2;
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
            // Just insert NULL as the port name and the verilog module should understand
            VerilogExpressions::const_iterator it;
            for (it = $1->begin(); it != $1->end(); ++it)
            {
                $$->push_back(new VerilogConn(NULL, *(*it)));
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

// Support very simple form of expressions, no unary/binary/conditional operations are
// supported. Thus, this is merged with "primary". STRINGS are also disallowed
expression:
    NUMBER                                  { $$ = new VerilogExpression(*$1); delete $1; }
    | IDENTIFIER                            { $$ = new VerilogExpression($1); }
    | IDENTIFIER range                      { $$ = new VerilogExpression($1, *$2); delete $2; }
    | concatenation                         { $$ = new VerilogExpression(*$1); delete $1; }
    ;
    
constant_expression:
    NUMBER                                  { $$ = new VerilogExpression(*$1); delete $1; }
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

namespace Thermal
{
    VerilogFileReader::VerilogFileReader(VerilogFile* file_)
        : m_file_(file_)
    {}
    
    VerilogFileReader::~VerilogFileReader()
    {
        delete m_file_;
    }

    bool VerilogFileReader::parse()
    {
        if(m_file_ != NULL)
        {
            yyin = fopen( m_file_->getFileName().c_str(), "r" );
            if( yyin == NULL ) 
            {
                cout << "VerilogFileReader::parse : [Error] Problem opening the input file: " << m_file_->getFileName() << "." << endl;
                throw -1;
            }
        } 
        else 
        {
            cout << "VerilogFileReader::parse : [Warning] File name not given for verilog file reader constructor, running from stdin" << endl;
        }
        
        return yyparse( m_file_ ) == 0;
    }
}

// Arguments must match those to yyparse
void yyerror(VerilogFile* file_, const char *s) {
	cout << "[Error] " << file_->getFileName() << ":" << line_num << " -> " << s << endl;
	// might as well halt now:
	exit(-1);
}