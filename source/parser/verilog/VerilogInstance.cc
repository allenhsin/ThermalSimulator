
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogInstance.h"

namespace Thermal
{
    using namespace LibUtil;

    VerilogInstance::VerilogInstance(const string& name_) 
        : VerilogItem(ITEM_INSTANCE), m_name_(name_)
    {}

    VerilogInstance::~VerilogInstance()
    {}

    
} // namespace Thermal
