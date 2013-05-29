
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>

#include "verilog/VerilogParser.h"

using namespace std;
using namespace VerilogParser;

extern string snazzle_awesome;
extern int snazzleparse();
extern FILE* snazzlein;

// File used to test the parser
int main(int argc, char** argv) 
{
    if (argc >= 3)
    {
        VerilogFileReader* file_reader = new VerilogFileReader();
    
        std::string top_name = argv[1];
    
        for (int i = 2; i < argc; ++i)
        {
            VerilogFile* file = new VerilogFile(argv[i]);
            file_reader->parse(file);
            cout << file->getFileName() + ": Parsing successful" << endl;
            delete file;
        }
        
        file_reader->elaborate(top_name);
        file_reader->dumpRawModules(cout);
        file_reader->dumpElabModules(cout);
        
        // cout << "Elaboration successful" << endl;
        // cout << "Printing post-elaboration contents:" << endl;
        // const VerilogModuleMap& modules = file_reader->getVerilogScope()->getElabModules();
        // for(VerilogModuleMap::const_iterator m_it = modules.begin(); m_it != modules.end(); ++m_it)
        // {
            // const VerilogModule* module = m_it->second;
            // const VerilogItemMap* items = module->getItemMap();
            // cout << "    " << module->getName() << endl;            
            // for(VerilogItemMap::const_iterator i_it = items->begin(); i_it != items->end(); ++i_it)
                // cout << "        " << (*i_it).second->toString() << endl;
        // }
        
        // VerilogItems* items = file_reader->getFlattenedItems("AND_X1M_A12TR", ".");
        // for(VerilogItems::const_iterator it = items->begin(); it != items->end(); ++it)
        // {
            // VerilogItem* item = (*it);
            // cout << item->toString() << endl;
        // }
        // deletePtrVector<VerilogItem>(items);
        
        
        delete file_reader;            
        exit(0);
    }
    else
    {
        cout << "USAGE: test TOP_LEVEL_MODULE VERILOG_FILES" << endl;
        exit(-1);
    }
}
