
// This will be a snazzle parser, so prefix everything with snazzle
%define api.prefix "snazzle"

%{
#include <cstdio>
#include <iostream>

using namespace std;

#define yyin snazzlein

// stuff from flex that bison needs to know about:
int yylex();
int yyparse();

extern int snazzle_line_num;

string snazzle_awesome; 
void yyerror(const char *s);
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
}

// define the constant-string tokens:
%token SNAZZLE TYPE
%token END ENDL

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%%

// the first rule defined is the highest-level rule, which in our
// case is just the concept of a whole "snazzle file":
snazzle:
	header template body_section footer 
    { 
        snazzle_awesome = "SNAZZLE";
        cout << "done with a snazzle file!" << endl; 
    }
	;
header:
	SNAZZLE FLOAT ENDLS { cout << "reading a snazzle file version " << $2 << endl; }
	;
template:
	typelines
	;
typelines:
	typelines typeline
	| typeline
	;
typeline:
	TYPE STRING ENDLS { cout << "new defined snazzle type: " << $2 << endl; }
	;
body_section:
	body_lines
	;
body_lines:
	body_lines body_line
	| body_line
	;
body_line:
	INT INT INT INT STRING ENDLS { cout << "new snazzle: " << $1 << $2 << $3 << $4 << $5 << endl; }
	;
footer:
	END ENDLS
	;
    
ENDLS:
    ENDLS ENDL
    | ENDL
    ;

%%

void yyerror(const char *s) {
	cout << "EEK, parse error on line " << snazzle_line_num << "!  Message: " << s << endl;
	// might as well halt now:
	exit(-1);
}