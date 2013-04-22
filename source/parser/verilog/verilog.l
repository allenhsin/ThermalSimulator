
/* This will be a verilog parser, so prefix everything with verilog */
%option prefix="verilog"

/* Don't need yywrap */
%option noyywrap

/* Don't need unput */
%option nounput

%{
#include <iostream>

#include "VerilogFileReader.h"
#include "VerilogFile.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"
#include "yacc.verilog.hh"

using namespace std;
using namespace Thermal;

#define YY_DECL extern "C++" int yylex()
#define yylval veriloglval

int line_num = 1;
void yyerror(VerilogFile* file_, const char *s);
%}

%%

\n              { ++line_num; }

"assign"        { return ASSIGN;        }
"defparam"      { return DEFPARAM;      }
"endgenerate"   { return ENDGENERATE;   }
"endmodule"     { return ENDMODULE;     }
"generate"      { return GENERATE;      }
"genvar"        { return GENVAR;        }
"inout"         { return INOUT;         }
"input"         { return INPUT;         }
"module"        { return MODULE;        }
"output"        { return OUTPUT;        }
"parameter"     { return PARAMETER;     }
"wire"          { return WIRE;          }

"!"             { return yytext[0];     }
"#"             { return yytext[0];     }
"$"             { return yytext[0];     }
"%"             { return yytext[0];     }
"&"             { return yytext[0];     }
"("             { return yytext[0];     }
")"             { return yytext[0];     }
"*"             { return yytext[0];     }
"+"             { return yytext[0];     }
","             { return yytext[0];     }
"-"             { return yytext[0];     }
"."             { return yytext[0];     }
"/"             { return yytext[0];     }
":"             { return yytext[0];     }
";"             { return yytext[0];     }
"<"             { return yytext[0];     }
"="             { return yytext[0];     }
">"             { return yytext[0];     }
"?"             { return yytext[0];     }
"@"             { return yytext[0];     }
"["             { return yytext[0];     }
"]"             { return yytext[0];     }
"^"             { return yytext[0];     }
"|"             { return yytext[0];     }
"~"             { return yytext[0];     }

 /* numbers */
[0-9]+\.[0-9]+              { yylval.fval = atof(yytext); return FLOAT; }
[0-9]+                      { yylval.ival = atoi(yytext); return INT; }

 /* identifiers */
[a-zA-Z][_a-zA-Z0-9]*       { yylval.sval = strdup(yytext); return STRING; }

 /* ignore white spaces */
[ \t]                                   { ; }

 /* ignore comments in the lexer */
\/\/.*\n                                { ; }

 /* unrecognized tokens */
.                                       { cout << "[Error] line " << line_num << ": Unrecognized token!" << endl; exit(-1); }

%%