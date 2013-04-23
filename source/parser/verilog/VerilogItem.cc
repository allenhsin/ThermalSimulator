
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogItem.h"

namespace Thermal
{
    VerilogItem::VerilogItem(VerilogItemType type_)
        : m_type_(type_)
    {}
    
    VerilogItem::~VerilogItem()
    {}
    
} // namespace Thermal
