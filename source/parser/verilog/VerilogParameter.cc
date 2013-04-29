
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogParameter.h"
#include "VerilogScope.h"

namespace VerilogParser
{
    using namespace LibUtil;
    using namespace std;

    VerilogParameter::VerilogParameter(const string& identifier_, const VerilogExpression& value_)
        : VerilogItem(ITEM_PARAMETER, identifier_), m_value_(value_)
    {}

    VerilogParameter::~VerilogParameter()
    {}

    void VerilogParameter::elaborateItem(VerilogScope* scope_)
    {
        m_value_.elaborate(scope_);
        if (!m_value_.isConstExpr())
            throw VerilogException("Parameter '" + getIdentifier() + " resolved to a" + 
                " non-constant expression: " + m_value_.toString());
    }
    
    std::string VerilogParameter::toString() const
    {
        return "Parameter -> " + getIdentifier() + " = " + m_value_.toString();
    }
    
    VerilogItem* VerilogParameter::clone() const
    {
        return new VerilogParameter(*this);
    }
    
    VerilogParameter::VerilogParameter(const VerilogParameter& param_)
        : VerilogItem(param_),  m_value_(param_.m_value_)
    {}
    
    
} // namespace VerilogParser
