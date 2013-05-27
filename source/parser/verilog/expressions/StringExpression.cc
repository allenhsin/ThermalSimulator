
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "StringExpression.h"
#include "../Number.h"
#include "../BitVector.h"

namespace VerilogParser
{
    using std::string;

    StringExpression::StringExpression(const string& str_)
        : Expression(STRING), RHExpression(STRING), m_str_(str_)
    {}

    StringExpression::StringExpression(const StringExpression& expr_)
        : Expression(STRING), RHExpression(STRING), m_str_(expr_.m_str_)
    {}
    
    StringExpression::~StringExpression()
    {}
    
    BitVector* StringExpression::elaborate(VerilogScope* /* scope_ */) const
    {
        // Return the string as a bit vector. Remember to prune the "" from the
        // two ends
        return BitVector::makeBitVector(m_str_.substr(1, m_str_.size() - 2));
    }
    
    string StringExpression::toString() const
    {
        return m_str_;
    }
    
    
} // namespace VerilogParser
