#ifndef VERILOG_PARSER_RAW_ASSIGN_H
#define VERILOG_PARSER_RAW_ASSIGN_H

#include <string>
#include <vector>

#include "RawItem.h"
#include "../expressions/Expression.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class VerilogScope;

    class RawAssign : public RawItem
    {
        public:
            RawAssign(const LHExpression& lh_expr_, const RHExpression& rh_expr_);
            virtual ~RawAssign();            

        public:
            // Accessors
            inline const LHExpression* getLHExpression() const { return m_lh_expr_; }
            inline const RHExpression* getRHExpression() const { return m_rh_expr_; }
            
            // Elaborate the item in the given verilog scope for the given module
            void elaborate(VerilogScope* scope_) const;
            // return as string
            inline std::string toString() const { return RawItem::toString(); }
            // Clone
            RawItem* clone() const;

        protected:
            // Protected copy constructor, use clone instead
            RawAssign(const RawAssign& net_);
            
        private:
            // left hand expression
            const LHExpression* m_lh_expr_;
            // right hand expression
            const RHExpression* m_rh_expr_;
    };
}
#endif
