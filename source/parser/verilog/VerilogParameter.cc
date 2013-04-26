
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogParameter.h"
#include "VerilogScope.h"

namespace Thermal
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
    
} // namespace Thermal
