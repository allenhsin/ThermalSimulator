
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "NumberExpression.h"
#include "../Number.h"
#include "../BitVector.h"

namespace VerilogParser
{
    using std::string;

    NumberExpression::NumberExpression(unsigned long num_)
        : Expression(NUMBER), RHExpression(NUMBER), m_number_(num_)
    {}
    
    NumberExpression::NumberExpression(const Number& num_)
        : Expression(NUMBER), RHExpression(NUMBER), m_number_(num_)
    {}

    NumberExpression::NumberExpression(const NumberExpression& num_)
        : Expression(NUMBER), RHExpression(NUMBER), m_number_(num_.m_number_)
    {}
    
    NumberExpression::~NumberExpression()
    {}
    
    BitVector* NumberExpression::elaborate(VerilogScope* /* scope_ */) const
    {        
        return BitVector::makeBitVector(m_number_.toULong(), m_number_.getWidth());
    }
    
    string NumberExpression::toString() const
    {
        return m_number_.toString();
    }
    
    
} // namespace VerilogParser
