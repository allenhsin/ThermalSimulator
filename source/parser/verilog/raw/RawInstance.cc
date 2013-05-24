
#include <cassert>
#include <stdio.h>

#include "RawInstance.h"
#include "../elaborated/ElabItem.h"

namespace VerilogParser
{
    using std::string;

    RawInstance::RawInstance(const string& identifier_, const SetValues& conns_) 
        : RawItem(RAW_ITEM_INST, identifier_)
    {
        m_conns_ = new SetValues(conns_);
        m_parameter_values_ = new SetValues();
    }

    RawInstance::~RawInstance()
    {
        deletePtrVector<SetValue>(m_conns_);
        deletePtrVector<SetValue>(m_parameter_values_);
    }
    
    // Set parameter values
    void RawInstance::setParameterValues(const SetValues& parameter_values_)
    {
        m_parameter_values_->insert(
                m_parameter_values_->end(), 
                parameter_values_.begin(), 
                parameter_values_.end()
            );
    }
    
    IndexedElabItems* RawInstance::elaborate(VerilogScope* scope_) const
    {
        IndexedElabItems* elab_nets = new IndexedElabItems(0);
        //TODO
        return elab_nets;
    }
    
    std::string RawInstance::toString() const
    {
        return RawItem::toString() + "(" + m_module_name_ + ")";
    }
    
    RawInstance* RawInstance::clone() const
    {
        return new RawInstance(*this);
    }
    
    RawInstance::RawInstance(const RawInstance& instance_) 
        : RawItem(instance_), m_module_name_(instance_.m_module_name_)
    {
        m_conns_ = new SetValues();
        m_parameter_values_ = new SetValues();
        
        // Make a deep copy of everything
        SetValues::const_iterator it;
        for(it = instance_.m_conns_->begin(); it != instance_.m_conns_->end(); ++it)
            m_conns_->push_back(new SetValue(**it));
        for(it = instance_.m_parameter_values_->begin(); it != instance_.m_parameter_values_->end(); ++it)
            m_parameter_values_->push_back(new SetValue(**it));
    }
    
} // namespace VerilogParser
