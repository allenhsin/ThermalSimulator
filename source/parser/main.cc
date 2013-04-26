
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>

#include "verilog/VerilogFileReader.h"
#include "verilog/VerilogFile.h"
#include "verilog/VerilogModule.h"
#include "verilog/VerilogItem.h"
#include "verilog/VerilogMisc.h"

using namespace std;
using namespace VerilogParser;

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

    if (argc == 2)
    {
        VerilogFile* file_ = new VerilogFile(argv[1]);
        VerilogFileReader* file_reader = new VerilogFileReader(file_);
        
        file_reader->parse();
        cout << "Parsing successful" << endl;
        //snazzleparse();
        file_reader->elaborate();
        cout << "Elaboration successful" << endl;

        cout << "Printing post-elaboration contents:" << endl;
        const VerilogModules* modules = file_reader->getFile()->getModules();
        for(VerilogModules::const_iterator m_it = modules->begin(); m_it != modules->end(); ++m_it)
        {
            const VerilogModule* module = *m_it;
            const VerilogItemMap* items = module->getItemMap();
            cout << "    " << module->getName() << endl;            
            for(VerilogItemMap::const_iterator i_it = items->begin(); i_it != items->end(); ++i_it)
                cout << "        " << (*i_it).second->toString() << endl;
        }        
        delete file_reader;            
        exit(0);
    }
    else
    {
        cout << "USAGE: test verilog_file" << endl;
        exit(-1);
    }
}
