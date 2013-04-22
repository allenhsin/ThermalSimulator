
#include <cstdio>
#include <iostream>

#include "generated/yacc.verilog.hh"

using namespace std;

extern string awesome;
extern FILE* verilogin;

int main() 
{
	// open a file handle to a particular file:
	FILE *myfile = fopen("a.snazzle.file", "r");
	// make sure it's valid:
	if (!myfile) {
		cout << "I can't open a.snazzle.file!" << endl;
		return -1;
	}
	// set flex to read from it instead of defaulting to STDIN:
	verilogin = myfile;

	// parse through the input until there is no more:
	// do {
        verilogparse();
	// } while (!feof(yyin));
    
    cout << awesome << endl;
	
}
