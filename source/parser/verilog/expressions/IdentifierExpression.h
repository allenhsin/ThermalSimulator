#ifndef VERILOG_PARSER_IDENTIFIER_EXPRESSION_H
#define VERILOG_PARSER_IDENTIFIER_EXPRESSION_H

#include <string>
#include <vector>

#include "Expression.h"
#include "../Number.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class Number;

    class IdentifierExpression : public LHExpression, public RHExpression
    {
        public:
            IdentifierExpression(const std::string& identifier_, const BitRange& range_);
            IdentifierExpression(const std::string& identifier_);
            virtual ~IdentifierExpression();
            
            // Return the expression as usable bit targets
            BitVector* elaborate(VerilogScope* scope_) const;
            // Returns the expression as a printable string
            std::string toString() const;
            // Clone function
            inline IdentifierExpression* clone() const { return new IdentifierExpression(*this); }
            
        protected:
            // Accessors
            inline virtual const std::string* accIdentifier() const { return &m_identifier_; }
            inline virtual const BitRange* accRange() const { return &m_range_; }
            
        private:
            // Copy constructor
            IdentifierExpression(const IdentifierExpression& expr_);
            
        private:
            // The identifier in the expression
            const std::string m_identifier_;
            // The range in the expression
            BitRange m_range_;
            // Whether the range is explicitly declared or found
            bool m_has_range_;
    };
}
#endif
