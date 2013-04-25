
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogInstance.h"
#include "VerilogExpression.h"

namespace Thermal
{
    using namespace LibUtil;

    VerilogInstance::VerilogInstance(const string& name_, const VerilogConns& conns_) 
        : VerilogItem(ITEM_INSTANCE), m_name_(name_)
    {
        m_conns_ = new VerilogConns();
        m_parameter_values_ = new VerilogConns();
        initializeConns(conns_);
    }

    VerilogInstance::~VerilogInstance()
    {
        deletePtrVector<VerilogConn>(m_conns_);
    }
    
    // Set parameter values
    void VerilogInstance::setParameterValues(const VerilogConns& parameter_values_)
    {
        m_parameter_values_->insert(
                m_parameter_values_->end(), 
                parameter_values_.begin(), 
                parameter_values_.end()
            );
    }

    // Initialize the instance connections
    void VerilogInstance::initializeConns(const VerilogConns& conns_)
    {
        m_conns_->insert(m_conns_->end(), conns_.begin(), conns_.end());
    }    
    
} // namespace Thermal
