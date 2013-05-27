
#include <iostream>

#include "IdentifierExpression.h"
#include "NumberExpression.h"
#include "../VerilogScope.h"
#include "../BitVector.h"
#include "../Bit.h"

namespace VerilogParser
{
    using std::string;

    IdentifierExpression::IdentifierExpression(const string& identifier_, const BitRange& range_)
        : Expression(IDENTIFIER), LHExpression(IDENTIFIER), RHExpression(IDENTIFIER), 
            m_identifier_(identifier_), m_range_(range_), m_has_range_(true)
    {}

    IdentifierExpression::IdentifierExpression(const string& identifier_)
        : Expression(IDENTIFIER),LHExpression(IDENTIFIER), RHExpression(IDENTIFIER), m_identifier_(identifier_), 
            m_range_(NumberExpression(0), NumberExpression(0)), m_has_range_(false)
    {}

    IdentifierExpression::IdentifierExpression(const IdentifierExpression& expr_)
        : Expression(IDENTIFIER), LHExpression(IDENTIFIER), RHExpression(IDENTIFIER), 
            m_identifier_(expr_.m_identifier_), m_range_(expr_.m_range_), m_has_range_(expr_.m_has_range_)
    {}
            
    IdentifierExpression::~IdentifierExpression()
    {}
    
    BitVector* IdentifierExpression::elaborate(VerilogScope* scope_) const
    {    
        const BitVector* symbol_bits = scope_->get(m_identifier_);
        BitVector* out = new BitVector();
        // If no range is declared, use the full range given by the symbol
        int low = symbol_bits->low();
        int high = symbol_bits->high();
        // If range is given, use the given range
        if (m_has_range_)
        {
            low = m_range_.low(scope_);
            high = m_range_.high(scope_);            
        }
        
        // Clone every bit
        for (int i = low; i <= high; ++i)
            out->addBack((*symbol_bits)[i]->clone());
        return out;
    }
    
    string IdentifierExpression::toString() const
    {
        if (m_has_range_)
            return m_identifier_ + m_range_.toString();
        return m_identifier_;
    }
    
    
} // namespace VerilogParser
