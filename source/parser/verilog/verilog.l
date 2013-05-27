
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

#include "raw/RawModule.h"
#include "raw/RawNet.h"
#include "raw/RawInstance.h"
#include "raw/RawParameter.h"

#include "expressions/StringExpression.h"
#include "expressions/IdentifierExpression.h"
#include "expressions/NumberExpression.h"
#include "expressions/ConcatLHExpression.h"
#include "expressions/ConcatRHExpression.h"
#include "expressions/BinaryExpression.h"

#include "VerilogScope.h"

#include "yacc.verilog.hh"

using namespace std;
using namespace VerilogParser;

#define YY_DECL extern "C++" int yylex()
#define yylval veriloglval

int line_num = 1;
void yyerror(VerilogFileReader* reader_, const char *s);
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
"{"             { return yytext[0];     }
"}"             { return yytext[0];     }
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
[_0-9]+                                 { yylval.num = new Number(yytext); return NUMBER; }
[_0-9]+"'"[bodh][ \t\n]*[a-fA-F_0-9]+   { yylval.num = new Number(yytext); return NUMBER; }

    /* identifiers */
[a-zA-Z_][a-zA-Z_0-9]*                  { yylval.sval = strdup(yytext); return IDENTIFIER; }
"\"".*"\""                              { yylval.sval = strdup(yytext); return STRING; }

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