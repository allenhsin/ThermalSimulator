
#include <cassert>
#include <stdio.h>

#include "VerilogElabParam.h"

namespace VerilogParser
{
    using namespace std;

    VerilogElabParam::VerilogElabParam(
                const std::string& identifier_,
                const std::string& value_
            )
        : VerilogElabItem(ITEM_PARAMETER, identifier_),
        m_value_(value_)
    {}

    VerilogElabParam::~VerilogElabParam()
    {}
    
    std::string VerilogElabParam::toString() const
    {
        return "Param -> " + getIdentifier() + " = " + m_value_;
    }
    
    VerilogElabItem* VerilogElabParam::clone() const
    {
        return new VerilogElabParam(*this);
    }
    
    VerilogElabParam::VerilogElabParam(const VerilogElabParam& param_)
        : VerilogElabItem(param_), m_value_(param_.m_value_)
    {}

    
} // namespace VerilogParser
