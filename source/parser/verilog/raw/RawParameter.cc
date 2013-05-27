
#include <iostream>

#include "RawParameter.h"
#include "../BitVector.h"
#include "../VerilogScope.h"
#include "../elaborated/ElabModule.h"
#include "../elaborated/ElabParam.h"

namespace VerilogParser
{
    using std::string;

    RawParameter::RawParameter(const string& identifier_, const RHExpression& value_)
        : RawItem(RAW_ITEM_PARAM, identifier_), m_value_(value_.clone())
    {}

    RawParameter::~RawParameter()
    {
        delete m_value_;
    }
    
    void RawParameter::elaborate(VerilogScope* scope_) const
    {
        const string& id = getIdentifier();        
        const BitVector* symb = NULL;
        ElabModule* module = scope_->getElabModule();
        // Check if the parameter value has been set in the instantiation
        // If it has not been set, set it
        if (module->getParams().count(id) == 0)
            symb = m_value_->elaborate(scope_);            
        // If it has already been set, just get its value
        else
            symb = (module->getParams()).at(id);
            
        // Check to see if the bit vector did not evaluate to a constant
        if(!symb->isConst())
            throw VerilogException("Parameter '" + getIdentifier() + "' did " +
                "not evaluate to a constant: " + m_value_->toString());
        
        // Add the parameter to the module
        module->addItem(new ElabParam(id, scope_, symb));
        // Put its vlaue in the scope
        scope_->add(getIdentifier(), symb);                    
    }

    std::string RawParameter::toString() const
    {
        return RawItem::toString() + "=" + m_value_->toString();
    }
    
    RawItem* RawParameter::clone() const
    {
        return new RawParameter(*this);
    }
    
    RawParameter::RawParameter(const RawParameter& param_)
        : RawItem(param_),  m_value_(param_.m_value_->clone())
    {}
    
    
} // namespace VerilogParser
