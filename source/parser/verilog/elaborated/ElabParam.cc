
#include <cassert>
#include <stdio.h>

#include "ElabParam.h"

namespace VerilogParser
{
    using std::string;

    ElabParam::ElabParam(const std::string& identifier_)
        : ElabItem(ELAB_ITEM_PARAM, identifier_)
    {}

    ElabParam::~ElabParam()
    {}
    
    std::string ElabParam::toString() const
    {
        return ElabItem::toString();
    }
    
    ElabItem* ElabParam::clone() const
    {
        return new ElabParam(*this);
    }
    
    ElabParam::ElabParam(const ElabParam& param_)
        : ElabItem(param_)
    {}

    
} // namespace VerilogParser
