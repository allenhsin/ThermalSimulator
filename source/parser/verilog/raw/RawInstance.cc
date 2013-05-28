
#include <cassert>
#include <stdio.h>

#include "RawInstance.h"
#include "RawModule.h"
#include "../VerilogScope.h"
#include "../BitVector.h"
#include "../expressions/Expression.h"
#include "../elaborated/ElabModule.h"
#include "../elaborated/ElabInstance.h"
#include "../elaborated/ElabAssign.h"

namespace VerilogParser
{
    using std::string;

    RawInstance::RawInstance(const string& identifier_, const SetValues& conns_) 
        : RawItem(RAW_ITEM_INST, identifier_)
    {
        m_conns_ = new SetValues(conns_);
        m_params_ = new SetValues();
    }

    RawInstance::~RawInstance()
    {
        deletePtrVector<SetValue>(m_conns_);
        deletePtrVector<SetValue>(m_params_);
    }
    
    // Set parameter values
    void RawInstance::setParameterValues(const SetValues& parameter_values_)
    {
        m_params_->insert(
                m_params_->end(), 
                parameter_values_.begin(), 
                parameter_values_.end()
            );
    }
    
    void RawInstance::elaborate(VerilogScope* scope_) const
    {
        // Get the current elab module
        ElabModule* module = scope_->getElabModule();
        
        SymbolMap param_map;
        // First, set parameters through symbol table, since the values of the
        // parameters need to be evaluated in the current scope
        for(SetValues::const_iterator it = m_params_->begin(); it != m_params_->end(); ++it)
            param_map[(*it)->getIdentifier()] = (*it)->getValue()->elaborate(scope_);
        
        // Push the current instance into the scoping stack
        scope_->push(this);
        
        // Elaborate the module and create new elaborated instance
        const RawModule* raw_module = scope_->getRawModule(m_module_name_);
        raw_module->elaborate(scope_, param_map);
        // Remember what the new module is
        // I don't really like this...have to set this manually
        ElabModule* new_module = scope_->getElabModule();
        // Set the module back to the old module
        scope_->setElabModule(module);        
        // Pop the current instance from the scoping stack
        scope_->pop();
        
        // Add the instance to the current module
        module->addItem(new ElabInstance(getIdentifier(), scope_, new_module));
        
        // Finally, make all connections using assigns, since the nets the connections
        // connect to need to be evaluatated in the current scope
        
        // Check the number of connections with the number of ports
        const Identifiers& port_list = raw_module->getPortList();
        unsigned int n_conns = m_conns_->size();
        unsigned int n_ports = port_list.size();
        if(n_conns > n_ports)
            throw VerilogException("Number of connections (" + makeString(n_conns) + ") " +
                "> number of module ports (" + makeString(n_ports) + ")");
        else if (n_conns != n_ports)
        {
            // TODO: I want to flag a warning if the number of specified connections is
            // different as well
        }
        
        unsigned int port_idx = 0;         
        for(SetValues::const_iterator it = m_conns_->begin(); it != m_conns_->end(); ++it)
        {
            const BitVector* port_expr = NULL;
            const BitVector* conn_expr = (*it)->getValue()->elaborate(scope_);
            // If the set value has an implicit ID, then we need to look up the port name we 
            // need to connect to from the module port list
            if ((*it)->hasImplicitID()) port_expr = scope_->get(getIdentifier() + "." + port_list[port_idx]);
            else port_expr = scope_->get(getIdentifier() + "." + (*it)->getIdentifier());

            module->addItem(new ElabAssign(scope_, *port_expr, *conn_expr));
            delete conn_expr;

            ++port_idx;
        }
    }
    
    std::string RawInstance::toString() const
    {
        return RawItem::toString() + "(" + m_module_name_ + ")";
    }

    RawInstance::RawInstance(const RawInstance& instance_) 
        : RawItem(instance_), m_module_name_(instance_.m_module_name_)
    {
        m_conns_ = new SetValues();
        m_params_ = new SetValues();
        
        // Make a deep copy of everything
        SetValues::const_iterator it;
        for(it = instance_.m_params_->begin(); it != instance_.m_params_->end(); ++it)
            m_params_->push_back(new SetValue(**it));
        for(it = instance_.m_conns_->begin(); it != instance_.m_conns_->end(); ++it)
            m_conns_->push_back(new SetValue(**it));
    }
    
} // namespace VerilogParser
