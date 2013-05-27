
#include <iostream>

#include "RawParameter.h"
#include "../BitVector.h"
#include "../VerilogScope.h"

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
    
    void RawParameter::elaborate(ElabModule* /* module_ */, VerilogScope* scope_) const
    {
        // Only need to do stuff if the parameter has not already been overwritten
        if(!scope_->has(getIdentifier()))
        {
            // Create the bit vector that links to the symbol
            BitVector* symb = m_value_->elaborate(scope_);

            // Check to see if the bit vector did not evaluate to a constant
            if(!symb->isConst())
                throw VerilogException("Parameter '" + getIdentifier() + "' did " +
                    "not evaluate to a constant: " + m_value_->toString());
            
            // Put its vlaue in the scope
            scope_->add(getIdentifier(), symb);
        }
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
