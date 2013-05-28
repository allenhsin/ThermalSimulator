
#include "VerilogMisc.h"
#include "BitVector.h"
#include "expressions/Expression.h"

namespace VerilogParser
{
    using std::string;
    
    BitRange::BitRange(const Expression& high_, const Expression& low_)
        : m_high_(high_.clone()), m_low_(low_.clone())
    {}
    
    BitRange::BitRange(const BitRange& range_)
        : m_high_(range_.m_high_->clone()), m_low_(range_.m_low_->clone())
    {}
    
    BitRange::~BitRange()
    {
        delete m_high_;
        delete m_low_;
    }
    
    int BitRange::high(VerilogScope* scope_) const
    {
        BitVector* high_bits = m_high_->elaborate(scope_);
        int out = high_bits->getConstNum();
        delete high_bits;        
        return out;
    }
    
    int BitRange::low(VerilogScope* scope_) const
    {
        BitVector* low_bits = m_low_->elaborate(scope_);
        int out = low_bits->getConstNum();
        delete low_bits;        
        return out;    
    }
    
        // // Sanity check the range
    // if (low > high)
    // {
        // throw VerilogException("Expressions elaborated to a bad range: " + 
            // m_range_.toString() + "([" + makeString(high) + ":" + makeString(low) + "]");
    // }        

    
    string BitRange::toString() const
    {
        return "[" + m_high_->toString() + ":" + m_low_->toString() + "]";
    }
    
    SetValue::SetValue(const Expression& value_)
        : m_implicit_(true), m_identifier_(), m_value_(value_.clone())
    {}

    SetValue::SetValue(const std::string& identifier_, const Expression& value_)
        : m_implicit_(false), m_identifier_(identifier_), m_value_(value_.clone())
    {}
    
    SetValue::SetValue(const SetValue& val_)
        : m_implicit_(val_.m_implicit_), m_identifier_(val_.m_identifier_), m_value_(val_.m_value_->clone())
    {}
    
    SetValue::~SetValue()
    {
        delete m_value_;
    }
    
    string SetValue::toString() const
    {
        return m_identifier_ + "=" + m_value_->toString();
    }
    
    VerilogException::VerilogException(const std::string& message_)
        : exception(), m_message_(message_)
    {}

    VerilogException::~VerilogException() throw()
    {}
    
} // namespace VerilogParser
