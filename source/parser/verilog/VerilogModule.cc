
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"

namespace Thermal
{
    using namespace LibUtil;

    VerilogModule::VerilogModule()
    {
        m_nets_ = new vector<VerilogNet*>();
        m_instances_ = new vector<VerilogInstance*>();
    }

    VerilogModule::~VerilogModule()
    {
        deletePtrVector<VerilogNet>(m_nets_);
        deletePtrVector<VerilogInstance>(m_instances_);

        m_nets_ = NULL;
        m_instances_ = NULL;
    }

    
} // namespace Thermal
