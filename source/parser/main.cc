
#include <cstdio>
#include <iostream>
#include <string>

#include "verilog/VerilogFileReader.h"
#include "verilog/VerilogFile.h"

using namespace std;
using namespace Thermal;

extern string snazzle_awesome;

extern int snazzleparse();

extern FILE* snazzlein;

// File used to test the parser
int main(int argc, char** argv) 
{
	// open a file handle to a particular file:
	//FILE *myfile2 = fopen("a.snazzle.file", "r");
	// make sure it's valid:
    // if(!myfile2)
    // {
		// cout << "I can't open a.snazzle.file!" << endl;
		// return -1;
	// }
	// set flex to read from it instead of defaulting to STDIN:
    // snazzlein = myfile2;

    VerilogFile* file_ = new VerilogFile(argv[1]);
    VerilogFileReader* file_reader = new VerilogFileReader(file_);
    file_reader->parse();
    //snazzleparse();
    file_reader->elaborate();

    //cout << snazzle_awesome << endl;

    delete file_reader;
    
}
