
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
#include "VerilogItem.h"
#include "VerilogMisc.h"

#include "VerilogNumber.h"
#include "VerilogParameter.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"

#include "VerilogScope.h"

#include "yacc.verilog.hh"

using namespace std;
using namespace Thermal;

#define YY_DECL extern "C++" int yylex()
#define yylval veriloglval

int line_num = 1;
void yyerror(VerilogFile* file_, const char *s);
%}

%s IN_COMMENT

%%

<INITIAL>{

    /* ignore comments in the lexer */
    
\n              { ++line_num;           }

    /* keywords */
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
"'"             { return yytext[0];     }

    /* verilog numbers */
[_0-9]+                                 { yylval.num = new VerilogNumber(yytext); return NUMBER; }
[_0-9]+"'"[bodh][ \t\n]*[a-fA-F_0-9]+   { yylval.num = new VerilogNumber(yytext); return NUMBER; }

    /* identifiers */
[a-zA-Z_][a-zA-Z_0-9]*                  { yylval.sval = strdup(yytext); return IDENTIFIER; }

    /* ignore white spaces */
[ \t]           { ; }

    /* Recognize comments  */
"//"[^\n]*      { ; }
"/*"            { BEGIN(IN_COMMENT);    }

    /* Other things */
.               { cout << "[Error] line " << line_num << ": Unrecognized token!" << endl; exit(-1); }

}

<IN_COMMENT>{
"*/"           { BEGIN(INITIAL);     }
[^*\n]+  
"*"
\n             { ++line_num;         }
}

%%