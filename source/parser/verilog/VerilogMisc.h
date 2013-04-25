#ifndef THERMAL_PARSER_VERILOG_MISC_H
#define THERMAL_PARSER_VERILOG_MISC_H

#include <string>
#include <vector>

// Various other data structures for verilog parsing
namespace Thermal
{
    class VerilogModule;
    class VerilogItem;

    class VerilogExpression;
    class VerilogParameter;
    class VerilogNet;
    class VerilogInstance;
    
    typedef std::pair<VerilogExpression, VerilogExpression> VerilogRange;
    typedef std::vector<char*> VerilogVariables;
    typedef std::pair<char*, VerilogExpression> VerilogConn;

    typedef std::vector<VerilogModule*> VerilogModules;
    typedef std::vector<VerilogItem*> VerilogItems;
    typedef std::vector<VerilogConn*> VerilogConns;
    typedef std::vector<VerilogExpression*> VerilogExpressions;
    
    typedef std::vector<VerilogParameter*> VerilogParameters;
    typedef std::vector<VerilogNet*> VerilogNets;
    typedef std::vector<VerilogInstance*> VerilogInstances;
    
}
#endif
