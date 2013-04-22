
// This will be a verilog parser, so prefix everything with verilog
%define api.prefix "verilog"

// Let yyparse take a verilog file pointer as an input
%parse-param { Thermal::VerilogFile* file_ }

// I am defining most of these following this guide:
// http://www.externsoft.ch/download/verilog.html

%{
#include <cstdio>
#include <iostream>

#include "VerilogFileReader.h"
#include "VerilogFile.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"

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
	int ival;
	float fval;
	char *sval;
    
    Thermal::VerilogModule* module;
    Thermal::VerilogNet* net;
    Thermal::VerilogInstance* instance;
}

// define the constant-string tokens:
%token SNAZZLE TYPE

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

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%start source_text

%%

source_text: 
    source_text description
    ;

description:
    module_declaration
    ;
    
module_declaration:
    MODULE identifier '(' module_port_list ')' ';' module_item_list ENDMODULE
    ;
        
module_item_list:
    /* empty */
    | module_item
    | module_item_list ';' module_item
    ;
    
module_port_list:
    /* empty */
    | module_port
    | module_port_list ',' module_port
    ;

// A module's port list can be a complete port declaration or just listing
// out all the port names
module_port:
    port_declaration        {}
    | STRING                { cout << "Specified a port name: " << $1 << endl; }
    ;

net_declaration:
    WIRE STRING                 { cout << "Declared a net: " << $2 << endl; }
    | WIRE bit_select STRING    { cout << "Declared a net: " << $3 << endl; }
    ;

port_declaration:
    port_dir net_declaration
    | port_dir bit_select STRING
    | port_dir STRING
    ;

parameter_declaration:
    PARAMETER STRING '=' expression_int { cout << "Made a parameter: " << $2 << " = " << $<ival>4 << endl; }
    ;
    
instance_declaration:
    /* empty */
    ;

port_dir:
    INPUT |
    OUTPUT |
    INOUT
    ;

module_body_statement:
    port_declaration
    | net_declaration
    | instance_declaration
    | parameter_declaration
            
expression_int:
    INT
    ;
    
bit_select:
    '[' expression_int ':' expression_int ']' {}
    | '[' expression_int ']' {}

// // the first rule defined is the highest-level rule, which in our
// // case is just the concept of a whole "snazzle file":
// snazzle:
	// header template body_section footer 
    // { 
        // cout << "done with a snazzle file!" << endl; 
    // }
	// ;
// header:
	// SNAZZLE FLOAT ENDLS { cout << "reading a snazzle file version " << $2 << endl; }
	// ;
// template:
	// typelines
	// ;
// typelines:
	// typelines typeline
	// | typeline
	// ;
// typeline:
	// TYPE STRING ENDLS { cout << "new defined snazzle type: " << $2 << endl; }
	// ;
// body_section:
	// body_lines
	// ;
// body_lines:
	// body_lines body_line
	// | body_line
	// ;
// body_line:
	// INT INT INT INT STRING ENDLS { cout << "new snazzle: " << $1 << $2 << $3 << $4 << $5 << endl; }
	// ;
// footer:
	// END ENDLS
	// ;
    
// ENDLS:
    // ENDLS ENDL
    // | ENDL
    // ;

%%

namespace Thermal
{
    VerilogFileReader::VerilogFileReader(VerilogFile* file_)
        : m_file_(file_)
    {}
    
    VerilogFileReader::~VerilogFileReader()
    {}

    bool VerilogFileReader::parse()
    {
        if(m_file_ != NULL)
        {
            yyin = fopen( m_file_->getFileName().c_str(), "r" );
            if( yyin == NULL ) 
            {
                cout << "rshFileReader::parse : [Error] Problem opening the input file: " << m_file_->getFileName() << "." << endl;
                throw -1;
            }
        } 
        else 
        {
            cout << "rshFileReader::parse : [Warning] File name not given for verilog file reader constructor, running from stdin" << endl;
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