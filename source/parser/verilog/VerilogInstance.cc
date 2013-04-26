
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogInstance.h"
#include "VerilogExpression.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogScope.h"

namespace Thermal
{
    using namespace LibUtil;

    VerilogInstance::VerilogInstance(const string& identifier_, const VerilogConns& conns_) 
        : VerilogItem(ITEM_INSTANCE, identifier_)
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
    
    void VerilogInstance::elaborateItem(VerilogScope* scope_)
    {
        // First find the verilog module it instantiates
        m_module_ = scope_->getModule(m_module_name_);
        
        // Resolve parameter overrides
        VerilogConns::const_iterator it;
        for (it = m_parameter_values_->begin(); it != m_parameter_values_->end(); ++it)
        {
            const string& identifier = (*it)->first;
            VerilogExpression& expr = (*it)->second;
            
            // Check to make sure the identifier exists as a parameter
            const VerilogItem* item = m_module_->getItem(identifier);
            if (item->getType() != ITEM_PARAMETER)
                throw VerilogException("cannot set parameter " + identifier + ", not a parameter");
            
            // Elaborate the expression
            expr.elaborate(scope_);
            if (!expr.isConstExpr())
                throw VerilogException("parameter " + identifier + " resolved to a non-constant expression: "
                    + expr.toString());
        }

        // Resolve port connections
        for (it = m_conns_->begin(); it != m_conns_->end(); ++it)
        {
            const string& identifier = (*it)->first;
            VerilogExpression& expr = (*it)->second;
            
            // Elaborate the expression
            expr.elaborate(scope_);

            // Check to make sure the identifier exists as a net, if it is a named port connect
            if (identifier.size() > 0)
            {
                const VerilogItem* item = m_module_->getItem(identifier);
                if (item->getType () != ITEM_NET)
                    throw VerilogException("invalid port connection: " + identifier + " is not a port");
                else
                {
                    VerilogPortType port_type = ((VerilogNet*) item)->getPortType();
                    if (port_type == PORT_NONE)
                        throw VerilogException("invalid port connection: " + identifier + " is not a port");                    
                }
            }
            // TODO: Check for port width mismatches
        }
    }
    
} // namespace Thermal
