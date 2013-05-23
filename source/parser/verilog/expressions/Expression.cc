
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "Expression.h"

namespace VerilogParser
{
    using std::string;

    Expression::Expression(Type type_)
        : m_type_(type_)
    {}

    Expression::~Expression()
    {}

    // By default, these things will check the type and call the real
    // accessor function
    const Number& Expression::getNumber() const
    {
        if (m_type_ != NUMBER)
            throw VerilogException("Bad expression number access");
        return *accNumber();
    }

    const string& Expression::getString() const
    {
        if (m_type_ != STRING)
            throw VerilogException("Bad expression string access");
        return *accString();
    }

    const string& Expression::getIdentifier() const
    {
        if (m_type_ != IDENTIFIER)
            throw VerilogException("Bad expression identifier access");
        return *accIdentifier();
    }

    const BitRange& Expression::getRange() const
    {
        if (m_type_ != IDENTIFIER)
            throw VerilogException("Bad expression range access");
        return *accRange();
    }

    const LHExpressions& Expression::getLHConcat() const
    {
        if (m_type_ != LHCONCAT)
            throw VerilogException("Bad expression lh concat access");
        return *accLHConcat();        
    }

    const RHExpressions& Expression::getRHConcat() const
    {
        if (m_type_ != RHCONCAT)
            throw VerilogException("Bad expression rh concat access");
        return *accRHConcat();        
    }

    const Expression& Expression::getOpExpr0() const
    {
        if (m_type_ != BINARY)
            throw VerilogException("Bad expression operator 0 access");
        return *accOpExpr0();     
    }
    
    const Expression& Expression::getOpExpr1() const
    {
        if (m_type_ != BINARY)
            throw VerilogException("Bad expression operator 1 access");
        return *accOpExpr1();         
    }
    
    const BinOperator& Expression::getBinOp() const
    {
        if (m_type_ != BINARY)
            throw VerilogException("Bad expression binary operator access");
        return *accBinOp();         
    }
    
    LHExpression::LHExpression(Type type_)
        : Expression(type_)
    {}

    RHExpression::RHExpression(Type type_)
        : Expression(type_)
    {}
    
} // namespace VerilogParser
