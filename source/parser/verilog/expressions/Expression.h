#ifndef VERILOG_PARSER_EXPRESSION_H
#define VERILOG_PARSER_EXPRESSION_H

#include <string>
#include <vector>

#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class Number;
    class BitVector;
    class VerilogScope;

    class Expression
    {
        public:
            enum Type
            {
                INVALID,
                NUMBER,     //RH
                STRING,     //RH
                IDENTIFIER, //LH, RH
                LHCONCAT,   //LH
                RHCONCAT,   //RH
                BINARY      //RH
            };
    
        public:
            Expression(Type m_type_);
            virtual ~Expression();
            
            // Return the expression as usable bit targets
            virtual BitVector* elaborate(VerilogScope* scope_) const = 0;
            // Returns the expression as a printable string
            virtual std::string toString() const = 0;
            // Clone function
            virtual Expression* clone() const = 0;
            
            // Accessor functions
            const Number& getNumber() const;
            const std::string& getString() const;
            const std::string& getIdentifier() const;
            const BitRange& getRange() const;
            const LHExpressions& getLHConcat() const;
            const RHExpressions& getRHConcat() const;            
            const Expression& getOpExpr0() const;
            const Expression& getOpExpr1() const;
            const BinOperator& getBinOp() const;
            
        protected:
            // Method meant to be overwritten as accessor. If the default version is
            // called, this will just end up segfaulting
            inline virtual const Number* accNumber() const { return NULL; }
            inline virtual const std::string* accString() const { return NULL; }
            inline virtual const std::string* accIdentifier() const { return NULL; }
            inline virtual const BitRange* accRange() const { return NULL; }
            inline virtual const LHExpressions* accLHConcat() const { return NULL; } 
            inline virtual const RHExpressions* accRHConcat() const { return NULL; }             
            inline virtual const Expression* accOpExpr0() const { return NULL; }
            inline virtual const Expression* accOpExpr1() const { return NULL; }
            inline virtual const BinOperator* accBinOp() const { return NULL; }
            
        protected:
            // Copy constructor
            Expression(const Expression& expr_);
            
        private:
            // Type of the expression
            const Type m_type_;
    };
    
    class RHExpression : public virtual Expression
    {
        public:
            RHExpression(Type m_type_);
        
        public:
            // Clone function
            virtual RHExpression* clone() const = 0;

        protected:
            // Copy constructor
            RHExpression(const RHExpression& expr_);
        };
    
    class LHExpression : public virtual Expression
    {
        public:
            LHExpression(Type m_type_);    

        public:
            // Clone function
            virtual LHExpression* clone() const = 0;

        protected:
            // Copy constructor
            LHExpression(const LHExpression& expr_);
    };
    
}
#endif
