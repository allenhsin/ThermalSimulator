
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogInstance.h"
#include "VerilogExpression.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogScope.h"

namespace VerilogParser
{
    using namespace LibUtil;

    VerilogInstance::VerilogInstance(const string& identifier_, const VerilogConns& conns_) 
        : VerilogItem(ITEM_INSTANCE, identifier_)
    {
        m_conns_ = new VerilogConns(conns_);
        m_parameter_values_ = new VerilogConns();
    }

    VerilogInstance::~VerilogInstance()
    {
        deletePtrVector<VerilogConn>(m_conns_);
        deletePtrVector<VerilogConn>(m_parameter_values_);
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

    void VerilogInstance::elaborateItem(VerilogScope* scope_)
    {
        // Full module name in order to look for post-elaboration name lookup
        string full_name = elaborateParamVals(scope_);
        elaborateTargetModule(scope_, full_name);
        elaboratePortConns(scope_);
    }
    
    string VerilogInstance::elaborateParamVals(VerilogScope* scope_)
    {
        // Full module name in order to look for post-elaboration name lookup
        string full_name = m_module_name_;        
        // Elaborate all module overrides
        VerilogConns::const_iterator it;
        for (it = m_parameter_values_->begin(); it != m_parameter_values_->end(); ++it)
        {
            const string& identifier = (*it)->first;
            VerilogExpression& expr = (*it)->second;
            
            // Elaborate the expression
            expr.elaborate(scope_);
            if (!expr.isConstExpr())
                throw VerilogException("parameter " + identifier + " resolved to a non-constant expression: "
                    + expr.toString());
                    
            full_name += "_" + identifier + "_" + expr.toString();
        }
        
        return full_name;
    }
    
    void VerilogInstance::elaborateTargetModule(VerilogScope* scope_, const string& full_name_)
    {
         // Check to see if the module this instantiates already has been elaborated
        if (scope_->hasElabModule(full_name_))
            m_module_ = scope_->getElabModule(full_name_);
        // If not, create a copy of the raw module and elaborate it with the given parameter values
        else
        {
            m_module_ = new VerilogModule(scope_->getRawModule(m_module_name_), *m_parameter_values_);
            m_module_->elaborate(scope_);
            
            // Check that these parameters exist in the target module
            VerilogConns::const_iterator it;
            for (it = m_parameter_values_->begin(); it != m_parameter_values_->end(); ++it)
            {
                const string& identifier = (*it)->first;
                // Check to make sure the identifier exists as a parameter
                const VerilogItem* item = m_module_->getItem(identifier);
                if (item->getType() != ITEM_PARAMETER)
                    throw VerilogException("cannot set parameter " + identifier + ", not a parameter");
            }
        }
        std::cout << full_name_ << std::endl;
    }
    
    void VerilogInstance::elaboratePortConns(VerilogScope* scope_)
    {           
        // Resolve port connections
        VerilogConns::iterator conn_it;
        for (conn_it = m_conns_->begin(); conn_it != m_conns_->end(); ++conn_it)
        {
            const string& identifier = (*conn_it)->first;
            VerilogExpression& expr = (*conn_it)->second;
            
            // Elaborate the expression
            expr.elaborate(scope_);
            
            // If it is a non-named port connect, find the identifier through the port list
            if (identifier.size() == 0)
                throw VerilogException("non-named port connections currently unsupported");

            // Check to make sure the identifier exists as a net, if it is a named port connect
            const VerilogItem* item = m_module_->getItem(identifier);
            if (item->getType () != ITEM_NET)
                throw VerilogException("invalid port connection: " + identifier + " is not a port");
            else
            {
                VerilogPortType port_type = ((VerilogNet*) item)->getPortType();
                if (port_type == PORT_NONE)
                    throw VerilogException("invalid port connection: " + identifier + " is not a port");                    
            }
            
            // Expression should have evaluated to a NUMBER or IDENTIFIER
            if (expr.getType() == VerilogExpression::NUMBER)
                throw VerilogException("port connect to number currently unsupported");
            else if (expr.getType() == VerilogExpression::IDENTIFIER_RANGE)
            {
                // Check for port width mismatch
                const VerilogNet* port = (const VerilogNet*) item;
                const VerilogRange& range_port = port->getRange();
                const VerilogRange& range_net = expr.getRange();
                
                unsigned int width_port = range_port.first.getConstExpr() - range_port.second.getConstExpr();
                unsigned int width_net = range_net.first.getConstExpr() - range_net.second.getConstExpr();
                
                if (width_port != width_net)
                    throw VerilogException("Port width mismatch: " + identifier + " and " + expr.getIdentifier());
            }
            else throw VerilogException("bad expression in port connect: " + expr.toString());
        }
    }
    
    std::string VerilogInstance::toString() const
    {
        return "Instance -> " + getIdentifier() + " (" + m_module_->getName() + ")";
    }
    
    VerilogInstance* VerilogInstance::clone() const
    {
        return new VerilogInstance(*this);
    }
    
    VerilogInstance::VerilogInstance(const VerilogInstance& instance_) 
        : VerilogItem(instance_), m_module_name_(instance_.m_module_name_), m_module_(instance_.m_module_)
    {
        m_conns_ = new VerilogConns();
        m_parameter_values_ = new VerilogConns();
        
        // Make a deep copy of everything
        VerilogConns::const_iterator it;
        for(it = instance_.m_conns_->begin(); it != instance_.m_conns_->end(); ++it)
            m_conns_->push_back(new VerilogConn(**it));
        for(it = instance_.m_parameter_values_->begin(); it != instance_.m_parameter_values_->end(); ++it)
            m_parameter_values_->push_back(new VerilogConn(**it));
    }
    
    VerilogInstance* VerilogInstance::getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const
    {
        VerilogInstance* c = (VerilogInstance*) VerilogItem::getFlattenedClone(hier_, hier_sep_);
        // Need to change the connections to use flat naming
        VerilogConns* conns = c->m_conns_;
        VerilogConns* params = c->m_parameter_values_;
        for (VerilogConns::const_iterator it = conns->begin(); it != conns->end(); ++it)
        {
            // Port name
            (*it)->first = c->getIdentifier() + hier_sep_ + (*it)->first;
            // Rebase the expression as well
            (*it)->second.useHierName(hier_);
        }
        
        for (VerilogConns::const_iterator it = params->begin(); it != params->end(); ++it)
        {
            // Parameter name
            (*it)->first = c->getIdentifier() + hier_sep_ + (*it)->first;
            // Rebase the expression as well
            (*it)->second.useHierName(hier_);
        }
        return c;
    }
    
    
} // namespace VerilogParser
