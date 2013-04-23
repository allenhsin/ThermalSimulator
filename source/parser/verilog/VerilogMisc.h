#ifndef THERMAL_PARSER_VERILOG_MISC_H
#define THERMAL_PARSER_VERILOG_MISC_H

#include <string>
#include <vector>

// Various other data structures for verilog parsing
namespace Thermal
{
    class VerilogModule;
    class VerilogItem;

    class VerilogParameter;
    class VerilogNet;
    class VerilogInstance;
    
    typedef int VerilogExpression;
    typedef std::pair<int, int> VerilogRange;
    typedef std::vector<char*> VerilogVariables;

    typedef std::vector<VerilogModule*> VerilogModules;
    typedef std::vector<VerilogItem*> VerilogItems;
    
    typedef std::vector<VerilogParameter*> VerilogParameters;
    typedef std::vector<VerilogNet*> VerilogNets;
    typedef std::vector<VerilogInstance*> VerilogInstances;
    
}
#endif
