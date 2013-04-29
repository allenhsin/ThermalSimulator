#ifndef THERMAL_PARSER_VERILOG_EXPRESSION_H
#define THERMAL_PARSER_VERILOG_EXPRESSION_H

#include <string>
#include <vector>

#include "VerilogMisc.h"
#include "VerilogNumber.h"

// A class representing a verilog module
namespace VerilogParser
{
    class VerilogScope;
    
    enum VerilogBinOp
    {
        BOP_PLUS,
        BOP_MINUS,
        BOP_TIMES,
        BOP_DIV,
        BOP_MOD
    };

    class VerilogExpression
    {
        public:
            enum Type
            {
                NUMBER,
                CONCAT,
                IDENTIFIER,
                IDENTIFIER_RANGE,
                BINARY_EXPRESSION
            };
    
        public:
            VerilogExpression(const VerilogNumber& num_);
            VerilogExpression(const VerilogExpressions& concat_);
            VerilogExpression(const std::string& identifier_);
            VerilogExpression(const std::string& identifier_, const VerilogRange& range_);
            VerilogExpression(const VerilogExpression& expr0_, VerilogBinOp bin_op, const VerilogExpression& expr1_);
            virtual ~VerilogExpression();
            
            void elaborate(VerilogScope* scope_);        

        public:
            // Copy constructor
            VerilogExpression(const VerilogExpression& expr_);
                        
            inline Type getType() const { return m_type_; }
            inline const VerilogNumber& getNumber() const { return *m_val_num_; }
            inline const VerilogExpressions& getConcat() const { return *m_val_concat_; }
            inline const std::string& getIdentifier() const { return m_val_identifier_; }
            inline const VerilogRange& getRange() const { return *m_val_range_; }            
            inline bool isConstExpr() const { return m_const_expr_; }
            
            // Returns the expression as a printable string
            std::string toString() const;
        
        private:
            void deleteSelf();
            void copy(const VerilogExpression& expr_);
            // Evaluate a binary expression
            void evaluate();
            
        private:
            // Type of the expression
            Type m_type_;
            
            // Various stored types
            VerilogNumber* m_val_num_;
            VerilogExpressions* m_val_concat_;
            std::string m_val_identifier_;
            VerilogRange* m_val_range_;
            VerilogExpression* m_val_expr0_;
            VerilogExpression* m_val_expr1_;
            VerilogBinOp m_val_bin_op_;
            
            // Elaborated
            bool m_elaborated_;            
            // Is constant expression
            bool m_const_expr_;            
    };
}
#endif
