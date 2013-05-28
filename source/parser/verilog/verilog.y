
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
#include "VerilogMisc.h"

#include "raw/RawModule.h"
#include "raw/RawNet.h"
#include "raw/RawInstance.h"
#include "raw/RawParameter.h"

#include "expressions/EmptyExpression.h"
#include "expressions/StringExpression.h"
#include "expressions/IdentifierExpression.h"
#include "expressions/NumberExpression.h"
#include "expressions/ConcatLHExpression.h"
#include "expressions/ConcatRHExpression.h"
#include "expressions/BinaryExpression.h"

#include "VerilogScope.h"

#include <cstdlib>

using namespace std;
using namespace VerilogParser;

#define yyin verilogin

// stuff from flex that bison needs to know about:
int yylex();

extern int line_num;

extern FILE* yyin;
void yyerror(VerilogFileReader* reader_, const char *s);
%}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	char *sval;
    char cval;
    
    VerilogParser::Number* num;    

    VerilogParser::Expression* expr;
    VerilogParser::RHExpression* expr_rh;
    VerilogParser::LHExpression* expr_lh;
    
    VerilogParser::BitRange* range;
    VerilogParser::Identifiers* identifiers;
    VerilogParser::SetValue* conn;
    
    VerilogParser::PortType port_type;
    VerilogParser::NetType net_type;
    
    VerilogParser::Expressions* exprs;
    VerilogParser::RHExpressions* exprs_rh;
    VerilogParser::LHExpressions* exprs_lh;


    VerilogParser::SetValues* conns;
    
    VerilogParser::RawModule* module;
    VerilogParser::RawModules* modules;

    VerilogParser::RawItem* item;
    VerilogParser::RawItems* items;

    VerilogParser::RawNet* net;
    // VerilogParser::RawNets* nets;
    VerilogParser::RawInstance* instance;
    VerilogParser::RawInstances* instances;
    VerilogParser::RawParameter* parameter;
    // VerilogParser::RawParameters* parameters;
    
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
%token  <sval>          STRING

// Define non-terminal symbol data types
%type   <expr_rh>       constant_expression

%type   <expr_rh>       expression_rh
// %type   <expr_lh>       expression_lh

// %type   <exprs>         list_of_expressions
// %type   <exprs_lh>      list_of_expressions_lh
%type   <exprs_rh>      list_of_expressions_rh

%type   <identifiers>   list_of_identifiers

%type   <identifiers>   list_of_ports

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
        { $$ = new RawModule($2, *$4, *$7); delete $4; deletePtrVector<RawItem>($7); }
    ;
        
// The list of ports is online used to describe the port ordering if this module is instantiated
// using ordered (unnamed) port connections
list_of_ports:
    /* empty */                                     { $$ = new Identifiers(); }
    | IDENTIFIER                                    { $$ = new Identifiers(1, $1); }
    | list_of_ports ',' IDENTIFIER                  { $$ = $1; $1->push_back($3); }
    ;
    
module_item_list:
    /* empty */                                     { $$ = new RawItems(); }
    | module_item_list module_item                  { $$ = $1; $$->insert($$->end(), $2->begin(), $2->end()); delete $2; }
    ;

net_declaration:
    net_type list_of_identifiers ';'                    { $$ = RawNet::createRawNets(*$2, PORT_NONE, $1); delete $2; }
    | net_type range list_of_identifiers ';'            { $$ = RawNet::createRawNets(*$3, PORT_NONE, $1, *$2); delete $2; delete $3; }
    | port_dir list_of_identifiers ';'                  { $$ = RawNet::createRawNets(*$2, $1, NET_WIRE); delete $2; }
    | port_dir range list_of_identifiers ';'            { $$ = RawNet::createRawNets(*$3, $1, NET_WIRE, *$2); delete $2; delete $3; }
    | port_dir net_type list_of_identifiers ';'         { $$ = RawNet::createRawNets(*$3, $1, $2); delete $3; }
    | port_dir net_type range list_of_identifiers ';'   { $$ = RawNet::createRawNets(*$4, $1, $2, *$3); delete $3; delete $4; }
    ;    
    
range:
    '[' constant_expression ']'                             { $$ = new BitRange(*$2, *$2); delete $2; }
    | '[' constant_expression ':' constant_expression ']'   { $$ = new BitRange(*$2, *$4); delete $2; delete $4; }
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
    /* empty */                                         { $$ = new RawItems(); }
    | param_assignment                                  { $$ = new RawItems(1, $1); }
    | list_of_param_assignments ',' param_assignment    { $$ = $1; $$->push_back($3); }
    ;
        
param_assignment:
    IDENTIFIER '=' constant_expression                  { $$ = new RawParameter($1, *$3); delete $3; }
    ;

module_instantiation:
    IDENTIFIER parameter_value_assignment list_of_module_instances ';' 
        { 
            // I don't like how this is written, probably should rewrite the data
            // structure a bit to minimize the chance of accidental memory leaks
            RawInstances::const_iterator it;
            for (it = $3->begin(); it != $3->end(); ++it)
            {
                // Make a deep copy of $2 for every instance            
                SetValues* new_conns = new SetValues();
                SetValues::const_iterator c_it;
                for (c_it = $2->begin(); c_it != $2->end(); ++c_it)
                    new_conns->push_back(new SetValue(*(*c_it)));
                
                (*it)->setModuleName($1);
                (*it)->setParameterValues(*new_conns);
                
                delete new_conns;
            }

            $$ = new RawItems($3->begin(), $3->end());
            
            deletePtrVector<SetValue>($2);
            delete $3;
        }
    | IDENTIFIER list_of_module_instances ';' 
        { 
            RawInstances::const_iterator it;
            for (it = $2->begin(); it != $2->end(); ++it)
                (*it)->setModuleName($1);

            $$ = new RawItems($2->begin(), $2->end());
            delete $2;
        }
    ;

parameter_value_assignment:
    '#' '(' list_of_named_parameter_value_assignment ')'    { $$ = $3; }
    
list_of_named_parameter_value_assignment:
    /* empty */                                             { $$ = new SetValues(); }
    | list_of_named_connections                             { $$ = $1; }
    ;
    
// A list of named connections following the .IDENTIFIER(EXPRESSION) format
// Used for both port connections and parameter assignments
list_of_named_connections:
    named_connection                                        { $$ = new SetValues(); $$->push_back($1); }
    | list_of_named_connections ',' named_connection        { $$ = $1; $$->push_back($3); }
    ;
    
list_of_module_instances:
    IDENTIFIER '(' list_of_module_connections ')'                                 { $$ = new RawInstances(1, new RawInstance($1, *$3)); delete $3; }
    | list_of_module_instances IDENTIFIER '(' list_of_module_connections ')'      { $$ = $1; $$->push_back(new RawInstance($2, *$4)); delete $4; }
    ;
    
// Must used named ports for the whole thing or implicit ports, cannot mix between the two
list_of_module_connections:
    /* empty */                                             { $$ = new SetValues(); }
    | list_of_module_port_connections                       { $$ = $1; }
    | list_of_named_port_connections                        { $$ = $1; }
    ;
    
list_of_module_port_connections:
    list_of_expressions_rh
        { 
            // Convert a list of expressions to port connections
            $$ = new SetValues();
            // Create set values with implicit identifier
            RHExpressions::const_iterator it;
            for (it = $1->begin(); it != $1->end(); ++it)
            {
                $$->push_back(new SetValue(*(*it)));
                delete *it;
            }
            delete $1;
        }
    ;

list_of_named_port_connections:
    list_of_named_connections
    ;
    
named_connection:
    '.' IDENTIFIER '(' ')'                  { $$ = new SetValue($2, EmptyExpression()); }
    | '.' IDENTIFIER '(' expression_rh ')'  { $$ = new SetValue($2, *$4); delete $4; }
    ;
    
// TODO stack

// For now, I don't know what else is important in a verilog netlist file for us besides
// just module declarations
description:
    module_declaration
    ;
    
expression_rh:
    '(' expression_rh ')'                   { $$ = $2; }
    | NUMBER                                { $$ = new NumberExpression(*$1); delete $1; }
    | STRING                                { $$ = new StringExpression($1); }
    | IDENTIFIER                            { $$ = new IdentifierExpression($1); }
    | IDENTIFIER range                      { $$ = new IdentifierExpression($1, *$2); delete $2; }
    | expression_rh '+' expression_rh       { $$ = new BinaryExpression(*$1, "+", *$3); delete $1; delete $3; }
    | expression_rh '-' expression_rh       { $$ = new BinaryExpression(*$1, "-", *$3); delete $1; delete $3; }
    | expression_rh '*' expression_rh       { $$ = new BinaryExpression(*$1, "*", *$3); delete $1; delete $3; }
    | expression_rh '/' expression_rh       { $$ = new BinaryExpression(*$1, "/", *$3); delete $1; delete $3; }
    | expression_rh '%' expression_rh       { $$ = new BinaryExpression(*$1, "%", *$3); delete $1; delete $3; }
    | '{' list_of_expressions_rh '}'        { $$ = new ConcatRHExpression(*$2); delete $2; }
    ;
    
constant_expression:
    expression_rh                           { $$ = $1; }
    ;

list_of_expressions_rh:
    expression_rh                               { $$ = new RHExpressions(1, $1); }
    | list_of_expressions_rh ',' expression_rh  { $$ = $1; $$->push_back($3); }
    ;
    
list_of_identifiers:
    /* empty */                             { $$ = new Identifiers(); }
    | IDENTIFIER                            { $$ = new Identifiers(1, $1); }
    | list_of_identifiers ',' IDENTIFIER    { $$ = $1; $$->push_back($3); }
    ;

module_item:
    net_declaration
    | parameter_declaration
    | module_instantiation
    ;
    
// continuous_assign:
    // ASSIGN list_of_assignments ';'

// list_of_assignments:
    // assignment
    // ;
    
// assignment:
    // lh_expression '=' rh_expression    
        
// expression_lh:
    // '(' expression_lh ')'                   { $$ = $2; }
    // | IDENTIFIER                            { $$ = new IdentifierExpression($1); }
    // | IDENTIFIER range                      { $$ = new IdentifierExpression($1, *$2); delete $2; }
    // | '{' list_of_expressions_lh '}'        { $$ = new ConcatLHExpression(*$2); delete $2; }
    // ;        
        
// list_of_expressions_lh:
    // expression_lh                               { $$ = new LHExpressions(1, $1); }
    // | list_of_expressions_lh ',' expression_lh  { $$ = $1; $$->push_back($3); }
    // ;
    
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
    bool VerilogFileReader::parse(VerilogFile* file_)
    {
        line_num = 1;
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
}

// Arguments must match those to yyparse
void yyerror(VerilogFileReader* reader_, const char *s) {
	cout << "[Error] " << reader_->getCurFile()->getFileName() << ":" << line_num << " -> " << s << endl;
	// might as well halt now:
	exit(-1);
}
