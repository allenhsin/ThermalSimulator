#ifndef VERILOG_PARSER_RAW_TYPES_H
#define VERILOG_PARSER_RAW_TYPES_H

#include <exception>
#include <string>
#include <vector>
#include <map>

// Various other data structures for verilog parsing
namespace VerilogParser
{
    class RawItem;
    
    // Type of items inside modules
    enum RawItemType
    {
        RAW_ITEM_PARAM,
        RAW_ITEM_NET,
        RAW_ITEM_INST
    };
    
    typedef std::vector<RawItem*> RawItems;
    
}
#endif
