
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogParameter.h"

namespace Thermal
{
    using namespace LibUtil;
    using namespace std;

    VerilogParameter::VerilogParameter(const string& name_, const VerilogExpression value_)
        : VerilogItem(ITEM_PARAMETER),
        m_name_(name_),
        m_value_(value_)
    {}

    VerilogParameter::~VerilogParameter()
    {}

    
} // namespace Thermal
