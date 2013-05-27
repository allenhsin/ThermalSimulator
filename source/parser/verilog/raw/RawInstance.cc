
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
    
    void RawInstance::elaborate(ElabModule* module_, VerilogScope* scope_) const
    {
        SymbolMap param_map;
        // First, set parameters through symbol table
        SetValues::const_iterator it;
        for(it = m_params_->begin(); it != m_params_->end(); ++it)
        {
            const BitVector* bits = (*it)->getValue()->elaborate(scope_);
            scope_->add(getIdentifier() + "." + (*it)->getIdentifier(), bits);
            param_map[(*it)->getIdentifier()] = bits;
        }
        
        // Push the current instance into the scoping stack
        scope_->push(this);        
        // Elaborate the module and create new elaborated instance
        scope_->getRawModule(m_module_name_)->elaborate(scope_);
        // Add the instance to the module
        module_->addItem(new ElabInstance(getIdentifier(), scope_->getElabModule(), param_map));                
        // Pop the current instance from the scoping stack
        scope_->pop();
        
        // Finally, make all connections using assigns
        for(it = m_conns_->begin(); it != m_conns_->end(); ++it)
        {
            const BitVector* port_expr = scope_->get(getIdentifier() + "." + (*it)->getIdentifier());
            const BitVector* conn_expr = (*it)->getValue()->elaborate(scope_);
            module_->addItem(new ElabAssign(*port_expr, *conn_expr));
            delete conn_expr;
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
