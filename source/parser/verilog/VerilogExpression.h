#ifndef THERMAL_PARSER_VERILOG_EXPRESSION_H
#define THERMAL_PARSER_VERILOG_EXPRESSION_H

#include <string>
#include <vector>

#include "VerilogMisc.h"
#include "VerilogNumber.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogExpression
    {
        enum Type
        {
            NONE,
            NUMBER,
            CONCAT,
            IDENTIFIER,
            IDENTIFIER_RANGE
        };
    
        public:
            VerilogExpression();
            VerilogExpression(const VerilogNumber& num_);
            VerilogExpression(const VerilogExpressions& concat_);
            VerilogExpression(const std::string& identifier_);
            VerilogExpression(const std::string& identifier_, const VerilogRange& range_);
            virtual ~VerilogExpression();
                        
        public:
            // Copy constructor
            VerilogExpression(const VerilogExpression& expr_);
                        
            inline Type getType() const { return m_type_; }
            inline const VerilogNumber& getNumber() const { return *m_val_num_; }
            inline const VerilogExpressions& getConcat() const { return *m_val_concat_; }
            inline const std::string& getIdentifier() const { return m_val_identifier_; }
            inline const VerilogRange& getRange() const { return *m_val_range_; }
                        
        private:
            // Type of the expression
            const Type m_type_;
            // Various stored types
            VerilogNumber* m_val_num_;
            VerilogExpressions* m_val_concat_;
            std::string m_val_identifier_;
            VerilogRange* m_val_range_;            
    };
}
#endif
